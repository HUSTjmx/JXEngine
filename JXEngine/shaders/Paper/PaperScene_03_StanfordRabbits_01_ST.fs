#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 ScatterLight_Out;
layout(location = 2) out vec4 Transmittance_Out;
layout(location = 3) out vec4 FinalPos_Out;
in vec2 TexCoords;

uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 view_pre;
uniform mat4 projection;
uniform mat4 projection_inv;
uniform mat4 projection_pre;
uniform int FrameIndex;

//StepAlloc Alg Select
#ifndef STEP_ALLOC_METHOD
    #define STEP_ALLOC_METHOD 1
#endif

#ifndef SHADOW_STEP_ALLOC_METHOD
    #define SHADOW_STEP_ALLOC_METHOD 1
#endif

uniform sampler2D ScatterLight_In;
uniform sampler2D Transmittance_In;
uniform sampler2D FinalPos_In;

// TFRM
uniform float F_DIS;
uniform float F_ANG;
uniform float TEMPORAL_ACCELERATION_MUL;

#define float2 vec2
#define float3 vec3
#define float4 vec4
#define uint2 uvec2
#define uint3 uvec3
#define uint4 uvec4

// Fog Material Settings
#define SCATTERING  (0.7 * vec3(0.95, 0.5, 2.0))
#define ABSORPTION  (0.1 * vec3(0.75, 0.5, 0.0))

//0, 1 or 2
#define BASIC_ANIMATED_MEDIA 0
#define BREAK_POINT_TEST 1
#define BREAK_POINT_VALUE 2.0
// 0, 1
#define ROTATE_MEDIA 0

#define VOLUME_FILTERING_NEAREST 0

#define FBM_NOISE 1

#define BASIC_ANIMATED_NOISE 0

// Light
#define LIGHT_DENSITY 100.0
#define LIGHT_COLOR (vec3(1.0, 1.0, 1.0) * LIGHT_DENSITY)
#define LIGHT_SIZE 0.3

// Box
#define BOX_SIZE vec3(8.0, 8.0, 8.0)
// White Wall
#define OBJ_WHITEWALL 1.0
    const vec3 WhiteWallColor = vec3(0.7295, 0.7355, 0.729);
// Right Wall
#define OBJ_RIGHTWALL 2.0
    const vec3 RightWallColor = vec3(0.117, 0.4125, 0.115);
// Left Wall
#define OBJ_LEFTWALL 3.0
    const vec3 LeftWallColor = vec3(0.611, 0.0555, 0.062);
// YuShi Sphere
#define OBJ_YUSHI_SPHERE 4.0
    const vec3 YuShiPos = vec3(0.0, 0.0, -1.5);
    const float YushiRadius = 3.0; 
    const float rotateSpeed = 0.1;

// Wall Material
#define WALL_METALLIC 0.0
#define WALL_REFLECTANCE 0.1
#define WALL_ROUGHNESS 0.9

// Shade Settings
#define ShadowIntensity 1.0

// Phase Func Mode
#define PHASE_MODE 2.0
#define PHASE_G 0.5

float eyeAngle = 1.0;

float PhaseFunc(vec3 V, vec3 L, float g)
{

#ifndef PHASE_FUNCTION_MEDIA
    float mu = dot(V, L);
    if(PHASE_MODE < 0.5) return PhaseFunc_CON();
    if(PHASE_MODE < 1.5) return PhaseFunc_RAYLEIGH(mu);
    if(PHASE_MODE < 2.5) return PhaseFunc_MIE(mu, g);
#endif

#ifdef PHASE_FUNCTION_MEDIA
    float re = 0.0;
    float mu = dot(V, L);
    bool isFoveatedRegion = true;
    float op2 = 1.0 - IsFovealRegion(eyeAngle, 1.0, 1.0, isFoveatedRegion);

#if PHASE_FUNCTION_MEDIA == 1
    re =  PhaseFunc_RAYLEIGH(mu);
#elif PHASE_FUNCTION_MEDIA == 2
    re =  PhaseFunc_MIE(mu, g);
#elif PHASE_FUNCTION_MEDIA == 3

#ifdef PHASE_FUNCTION_BLEND_OP
    float op = clamp(PHASE_FUNCTION_BLEND_OP, 0.001, 1.0);
    re =  PhaseFunc_RAYLEIGH(mu) * op + PhaseFunc_MIE(mu, g) * (1.0 - op);
#else
    re = PhaseFunc_MIE(mu, g);
#endif

#else
    re = PhaseFunc_CON();
    
#endif

    re = mix(re, PhaseFunc_CON(), op2);
    return re;
#endif
}

float sdPlane(vec3 p, vec3 n /* normalized */, float h)
{
    return dot(p, n) + h;
}

float sdSphere(vec3 p, vec3 center, float s)
{
    return length(p - center) - s;
}

vec2 iBox(in vec3 ro, in vec3 rd, in vec3 rad)
{
    vec3 m = 1.0 / rd;
    vec3 n = m * ro;
    vec3 k = abs(m) * rad;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    return vec2(max(max(t1.x, t1.y), t1.z),
                min(min(t2.x, t2.y), t2.z));
}

vec2 iSphere( in vec3 ro, in vec3 rd, in vec4 sph )//from iq
{
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w * sph.w;
	float h = b * b - c;
	if( h < 0.0 ) return vec2(-1.0);
	h = sqrt(h);
	return vec2(-b - h, -b + h );
}


// Packed 32^3 bunny data as 32x32 uint where each bit represents density per voxel
#define BUNNY_VOLUME_SIZE 32
const uint packedBunny[1024] = uint[1024](0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,917504u,917504u,917504u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,1966080u,12531712u,16742400u,16742400u,16723968u,16711680u,8323072u,4128768u,2031616u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,6144u,2063360u,16776704u,33553920u,33553920u,33553920u,33553920u,33520640u,16711680u,8323072u,8323072u,2031616u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,268435456u,402653184u,134217728u,201326592u,67108864u,0u,0u,7168u,2031104u,16776960u,33554176u,33554176u,33554304u,33554176u,33554176u,33554176u,33553920u,16744448u,8323072u,4128768u,1572864u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,805306368u,939524096u,402653184u,478150656u,260046848u,260046848u,260046848u,125832192u,130055680u,67108608u,33554304u,33554304u,33554304u,33554304u,33554304u,33554304u,33554304u,33554176u,16776704u,8355840u,4128768u,917504u,0u,0u,0u,0u,0u,0u,0u,0u,0u,805306368u,1056964608u,1056964608u,528482304u,528482304u,260046848u,260046848u,260046848u,130039296u,130154240u,67108739u,67108807u,33554375u,33554375u,33554370u,33554368u,33554368u,33554304u,33554304u,16776960u,8330240u,4128768u,393216u,0u,0u,0u,0u,0u,0u,0u,0u,939524096u,1040187392u,1040187392u,520093696u,251658240u,251658240u,260046848u,125829120u,125829120u,130088704u,63045504u,33554375u,33554375u,33554375u,33554407u,33554407u,33554370u,33554370u,33554374u,33554310u,16776966u,4144642u,917504u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,15360u,130816u,262017u,4194247u,33554383u,67108847u,33554415u,33554407u,33554407u,33554375u,33554375u,33554318u,2031502u,32262u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,31744u,130816u,262019u,2097151u,134217727u,134217727u,67108863u,33554415u,33554407u,33554415u,33554383u,2097102u,982926u,32262u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,31744u,130816u,524263u,117964799u,127926271u,134217727u,67108863u,16777215u,4194303u,4194303u,2097151u,1048574u,65422u,16134u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3u,31751u,130951u,524287u,252182527u,261095423u,261095423u,59768830u,2097150u,1048574u,1048575u,262143u,131070u,65534u,16134u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,7u,31751u,130959u,503840767u,520617982u,529530879u,261095423u,1048575u,1048574u,1048574u,524286u,524287u,131070u,65534u,16134u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3u,1799u,32527u,134348750u,1040449534u,1057488894u,520617982u,51380223u,1048575u,1048575u,524287u,524287u,524287u,131070u,65534u,15886u,6u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,1536u,3968u,8175u,65535u,1006764030u,1040449534u,1057488894u,50855934u,524286u,524286u,524287u,524287u,524286u,262142u,131070u,65534u,32270u,14u,6u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3968u,8160u,8191u,805371903u,2080505854u,2114191358u,101187582u,34078718u,524286u,524286u,524286u,524286u,524286u,524286u,262142u,131070u,32766u,8078u,3590u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8128u,8176u,16383u,2013331455u,2080505854u,235143166u,101187582u,524286u,1048574u,1048574u,1048574u,1048574u,524286u,524286u,262142u,131070u,32766u,16382u,8070u,1024u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8160u,8184u,1879064574u,2013331455u,470024190u,67371006u,524286u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,524286u,262142u,65534u,16382u,8160u,1024u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8128u,8184u,805322750u,402718719u,134479870u,524286u,524286u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,65534u,16382u,16368u,1792u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3968u,8184u,16382u,131071u,262142u,524286u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,65534u,16382u,16368u,1792u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,1792u,8184u,16380u,65535u,262143u,524286u,524286u,1048574u,1048574u,1048575u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,65534u,16376u,16368u,1792u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8176u,16376u,32767u,262143u,524286u,1048574u,1048574u,1048575u,1048575u,1048575u,1048575u,1048574u,1048574u,524286u,262142u,32766u,16376u,8176u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,4032u,8184u,32766u,262142u,524286u,524286u,1048575u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,32766u,16376u,8176u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,384u,8184u,32766u,131070u,262142u,524286u,1048575u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,524286u,131070u,32766u,16368u,1920u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,4080u,32764u,65534u,262142u,524286u,524286u,524286u,1048574u,1048574u,524286u,524286u,524286u,262142u,131070u,32764u,8160u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,256u,16376u,32760u,131068u,262140u,262142u,524286u,524286u,524286u,524286u,524286u,262142u,131070u,65532u,16368u,3840u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3968u,32752u,65528u,131068u,262142u,262142u,262142u,262142u,262142u,262142u,262140u,131064u,32752u,7936u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8064u,32736u,65528u,131070u,131070u,131070u,131070u,131070u,131070u,65532u,32752u,8160u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3456u,16376u,32764u,65534u,65534u,65534u,32766u,32764u,16380u,4048u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,48u,2680u,8188u,8188u,8188u,8188u,4092u,120u,16u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,120u,248u,508u,508u,508u,248u,240u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,96u,240u,504u,504u,504u,240u,96u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,224u,224u,224u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u);

// Default noise erosion strength
float erosionStrength = 1.0;


float sampleBunny(float3 uvs)
{
    float3 voxelUvs = max(float3(0.0),min(uvs*float3(BUNNY_VOLUME_SIZE), float3(BUNNY_VOLUME_SIZE)-1.0));
    uint3 intCoord = uint3(voxelUvs);
    uint arrayCoord = intCoord.x + intCoord.z*uint(BUNNY_VOLUME_SIZE);
	
    // Very simple clamp to edge. It would be better to do it for each texture sample
    // before the filtering but that would be more expenssive...
    // Also adding small offset to catch cube intersection floating point error
    if(uvs.x<-0.001 || uvs.y<-0.001 || uvs.z<-0.001 ||
      uvs.x>1.001 || uvs.y>1.001 || uvs.z>1.001)
    	return 0.0;
   
    // 1 to use nearest instead
#if VOLUME_FILTERING_NEAREST
    // sample the uint representing a packed volume data of 32 voxel (1 or 0)
    uint bunnyDepthData = packedBunny[arrayCoord];
    float voxel = (bunnyDepthData & (1u<<intCoord.y)) > 0u ? 1.0 : 0.0;
#else
    uint3 intCoord2 = min(intCoord+uint3(1), uint3(BUNNY_VOLUME_SIZE-1));
    
    uint arrayCoord00 = intCoord.x  + intCoord.z *uint(BUNNY_VOLUME_SIZE);
    uint arrayCoord01 = intCoord.x  + intCoord2.z*uint(BUNNY_VOLUME_SIZE);
    uint arrayCoord10 = intCoord2.x + intCoord.z *uint(BUNNY_VOLUME_SIZE);
    uint arrayCoord11 = intCoord2.x + intCoord2.z*uint(BUNNY_VOLUME_SIZE);
    
    uint bunnyDepthData00 = packedBunny[arrayCoord00];
    uint bunnyDepthData01 = packedBunny[arrayCoord01];
    uint bunnyDepthData10 = packedBunny[arrayCoord10];
    uint bunnyDepthData11 = packedBunny[arrayCoord11];
        
    float voxel000 = (bunnyDepthData00 & (1u<<intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel001 = (bunnyDepthData01 & (1u<<intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel010 = (bunnyDepthData10 & (1u<<intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel011 = (bunnyDepthData11 & (1u<<intCoord.y)) > 0u ? 1.0 : 0.0;
    float voxel100 = (bunnyDepthData00 & (1u<<intCoord2.y)) > 0u ? 1.0 : 0.0;
    float voxel101 = (bunnyDepthData01 & (1u<<intCoord2.y)) > 0u ? 1.0 : 0.0;
    float voxel110 = (bunnyDepthData10 & (1u<<intCoord2.y)) > 0u ? 1.0 : 0.0;
    float voxel111 = (bunnyDepthData11 & (1u<<intCoord2.y)) > 0u ? 1.0 : 0.0;
    
    float3 d = voxelUvs - float3(intCoord);
    
    voxel000 = mix(voxel000,voxel100, d.y);
    voxel001 = mix(voxel001,voxel101, d.y);
    voxel010 = mix(voxel010,voxel110, d.y);
    voxel011 = mix(voxel011,voxel111, d.y);
    
    voxel000 = mix(voxel000,voxel010, d.x);
    voxel001 = mix(voxel001,voxel011, d.x);
    
    float voxel = mix(voxel000,voxel001, d.z);
#endif
    
    return voxel;
}

float rand(vec3 co)
{
    return -1.0 + fract(sin(dot(co.xy,vec2(12.9898 + co.z,78.233))) * 43758.5453) * 2.0;
}

float linearRand(vec3 uv)
{
	vec3 iuv = floor(uv);
	vec3 fuv = fract(uv);
	
	float v1 = rand(iuv + vec3(0,0,0));
	float v2 = rand(iuv + vec3(1,0,0));
	float v3 = rand(iuv + vec3(0,1,0));
	float v4 = rand(iuv + vec3(1,1,0));
	
	float d1 = rand(iuv + vec3(0,0,1));
	float d2 = rand(iuv + vec3(1,0,1));
	float d3 = rand(iuv + vec3(0,1,1));
	float d4 = rand(iuv + vec3(1,1,1));
	
	return mix(mix(mix(v1,v2,fuv.x),mix(v3,v4,fuv.x),fuv.y),
		       mix(mix(d1,d2,fuv.x),mix(d3,d4,fuv.x),fuv.y),
			   fuv.z);
}

float linearRandFBM(vec3 uv)
{
	float c = (linearRand(uv * 1.0) * 32.0 +
			   linearRand(uv * 2.0) * 16.0 + 
			   linearRand(uv * 4.0) * 8.0 + 
			   linearRand(uv * 8.0) * 4.0) / 32.0;
	return c * 0.5 + 0.5;
}

float3 worldPosTocubePos(float3 worldPos)
{
#if ROTATE_MEDIA == 1
    worldPos = worldPos * rotate_around_y(180 * sin(iTime * rotateSpeed));
#else
    worldPos = worldPos * rotate_around_y(90.0);
#endif
    // cube of world space size 4 with bottom face on the ground y=0
    return worldPos * (vec3(1.0, -1.0, 1.0) * -0.38) + float3(0.5,0.5,0.5);
}

// Get density for a position
float getDensity(float3 cubePos)
{
    cubePos = worldPosTocubePos(cubePos);
    float density = sampleBunny(cubePos);
    if(density==0.0) return 0.0;	// makes things a tad bit faster
#if FBM_NOISE
    float3 noiseUV = cubePos*12.0;
	#if BASIC_ANIMATED_NOISE
    noiseUV += iTime * float3(1.0,0.0,0.0);
	#endif
    density = density * max(0.0, 1.25*erosionStrength*linearRandFBM(noiseUV)*4.0-2.0); // more complex FBM noise
#else
    float3 noiseUV = cubePos*16.0;
	#if BASIC_ANIMATED_NOISE
    noiseUV += iTime * float3(1.0,0.0,0.0);
	#endif
    density = density * max(0.0, 0.5 + 0.5*erosionStrength*linearRand(noiseUV));
#endif
    return density;
}

vec2 opU(vec2 d1, vec2 d2)
{
    return (d1.x < d2.x) ? d1 : d2;
}

vec2 Map(in vec3 pos)
{
    vec2 res = vec2(1e10, 0.0);

    // Ground
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Top
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, -1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Back
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 0.0,- 1.0), 1.0), OBJ_WHITEWALL));
    // left
    res = opU(res, vec2(sdPlane(pos, vec3(1.0, 0.0, 0.0), 3.0), OBJ_LEFTWALL));
    // Right
    res = opU(res, vec2(sdPlane(pos, vec3(-1.0, 0.0, 0.0), 3.0), OBJ_RIGHTWALL));
    // YuShi
    res = opU(res, vec2(sdSphere(pos, YuShiPos, YushiRadius), OBJ_YUSHI_SPHERE));

    return res;
}

vec2 Map2(in vec3 pos)
{
    vec2 res = vec2(1e10, 0.0);

    // Ground
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Top
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, -1.0, 0.0), 3.0), OBJ_WHITEWALL));
    // Back
    res = opU(res, vec2(sdPlane(pos, vec3(0.0, 0.0,- 1.0), 1.0), OBJ_WHITEWALL));
    // left
    res = opU(res, vec2(sdPlane(pos, vec3(1.0, 0.0, 0.0), 3.0), OBJ_LEFTWALL));
    // Right
    res = opU(res, vec2(sdPlane(pos, vec3(-1.0, 0.0, 0.0), 3.0), OBJ_RIGHTWALL));
    return res;
}

vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0) * 0.5773 * 0.0005;
    return normalize( e.xyy * Map2( pos + e.xyy ).x + 
					  e.yyx * Map2( pos + e.yyx ).x + 
					  e.yxy * Map2( pos + e.yxy ).x + 
					  e.xxx * Map2( pos + e.xxx ).x );
}

vec2 RayCast(in vec3 ro, in vec3 rd)
{
    vec2 res = vec2(-1.0, -1.0);
    float tmin = 0.3;
    float tmax = 20.0;

    vec2 tb = iBox(ro, rd, BOX_SIZE);
    if(tb.x < tb.y && tb.y > 0.0 && tb.x < tmax)
    {
        tmin = max(tb.x, tmin);
        tmax = min(tb.y, tmax);

        float t = tmin;
        for(int i = 0; i < 70 && t < tmax; ++i)
        {
            vec2 h = Map(ro + rd * t);
            if(abs(h.x) < (0.0001 * t))
            {
                res = vec2(t, h.y);
                break;
            }
            t += h.x;
        }
        return res;
    }
    return res;
}

vec2 RayCast2(in vec3 ro, in vec3 rd)
{
    vec2 res = vec2(-1.0, -1.0);
    float tmin = 0.3;
    float tmax = 60.0;

    vec2 tb = iBox(ro, rd, BOX_SIZE);
    if(tb.x < tb.y && tb.y > 0.0 && tb.x < tmax)
    {
        tmin = max(tb.x, tmin);
        tmax = min(tb.y, tmax);

        float t = tmin;
        for(int i = 0; i < 70 && t < tmax; ++i)
        {
            vec2 h = Map2(ro + rd * t);
            if(abs(h.x) < (0.0001 * t))
            {
                res = vec2(t, h.y);
                break;
            }
            t += h.x;
        }
        return res;
    }
    return res;
}

float GetYushiColor(in vec3 p) {
	
	float res = 0.;
	
    vec3 c = p;
	for (int i = 0; i < 10; ++i) 
    {
        p =.7 * abs(p) / dot(p,p) - .7;
        p.yz = csqr(p.yz);
        p = p.zxy;
        res += exp(-19. * abs(dot(p,c)));
        
	}
	return res / 2.;
}

void getParticipatingMedia(out vec3 sigmaS, out vec3 sigmaE, in vec3 pos)
{
    sigmaS = SCATTERING;
    sigmaE = max_f_v3(0.0000001, ABSORPTION + SCATTERING);
#if BASIC_ANIMATED_MEDIA==1
    float r = floor(iTime);
#if BREAK_POINT_TEST == 1
    r = BREAK_POINT_VALUE;
#endif
    sigmaS = abs(5.0* float3(rand(float3(r,0.0,1.0)),rand(float3(r,0.0,5.0)),rand(float3(r,0.0,9.0))));
    float3 absorption = abs(2.0* float3(rand(float3(r,1.0,2.0)),rand(float3(r,1.0,7.0)),rand(float3(r,1.0,7.0))));
    sigmaE = sigmaS + absorption;
#elif BASIC_ANIMATED_MEDIA==2
    float r = iTime*0.2;
#if BREAK_POINT_TEST == 1
    r = BREAK_POINT_VALUE;
#endif
    sigmaS = abs(5.0* float3(sin(r*1.1),sin(r*3.3),sin(r*5.5)));
    float3 absorption = abs( 2.0* float3(sin(r*2.2),sin(r*4.4),sin(r*6.6)));
    sigmaE = sigmaS + absorption;
#endif
    sigmaE = max_f_v3(0.0000001, sigmaE);
}

float volumetricShadow(in vec3 from, in vec3 to)
{
#ifndef IS_SHOW_SHADOW
    return 1.0;
#endif

#ifdef IS_SHOW_SHADOW
#if IS_SHOW_SHADOW == false
    return 1.0;
#else

#if SHADOW_STEP_ALLOC_METHOD == 1
    bool isFoveal = true;
    float fovealIndensity = IsFovealRegion(eyeAngle, 1.0, 1.0, isFoveal);
    float numStep = ConstantStepAlloc(fovealIndensity, MIN_SHADOW_STEP_NUM, MAX_SHADOW_STEP_NUM);
#elif SHADOW_STEP_ALLOC_METHOD == 2
    float numStep = FoveatedStepAlloc(eyeAngle, MIN_SHADOW_STEP_NUM, MAX_SHADOW_STEP_NUM);
#else
    float numStep = MAX_SHADOW_STEP_NUM; // quality control. Bump to avoid shadow alisaing
#endif

    vec3 shadow = vec3(1.0);
    vec3 sigmaS = vec3(0.0);
    vec3 sigmaE = vec3(0.0);
    float dd = length(to-from) / numStep;
    for(float s = 0.5; s < (numStep - 0.1); s += 1.0)// start at 0.5 to sample at center of integral part
    {
        vec3 pos = from + (to - from) * (s / (numStep));
        getParticipatingMedia(sigmaS, sigmaE, pos);
        float density = getDensity(pos - YuShiPos);
        shadow = shadow * exp(-sigmaE * density * ShadowIntensity * dd);
    }
    return length(shadow) * 0.8;
#endif
#endif
    //return max_v3_elem(shadow);
}


vec3 evaluateLightWithPhase(in vec3 pos)
{
    vec3 light_sum = vec3(0.0);
    vec3 V = normalize(pos - viewPosWS);
    for(int i = 0; i < pointLightsNum; ++i)
        light_sum += GetPointLightIllumiance(i, pos) * 
        PhaseFunc(V, normalize(pointLights[i].position - viewPosWS), PHASE_G) * 
        volumetricShadow(pos, pointLights[i].position);
        
    
    return light_sum;
}


vec3 GetColor(in float ID, in vec3 ro, in vec3 rd, inout vec4 pre_pos, inout vec4 pre_scat, inout vec4 pre_transmittance)
{
    if(ID < 3.5)
    {
        pre_scat = vec4(0.0, 0.0, 0.0, 1.0);
        pre_transmittance = vec4(1.0, 1.0, 1.0, 0.0);
    }
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    else
    {
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, YushiRadius));
        float t = tmm.x;

#if STEP_ALLOC_METHOD == 1
        bool isFoveal = true;
        float fovealIndensity = IsFovealRegion(eyeAngle, 1.0, 1.0, isFoveal);
        float stepsNum = ConstantStepAlloc(fovealIndensity, MIN_STEP_NUM, MAX_STEP_NUM);
         //float dt = .2 - .195*cos(iTime*.05);//animated
#elif STEP_ALLOC_METHOD == 2
        float stepsNum = FoveatedStepAlloc(eyeAngle, MIN_STEP_NUM, MAX_STEP_NUM);
#else
        float stepsNum = MAX_STEP_NUM;
#endif 

        //float dt = .3;     //float dt = .2 - .195*cos(iTime*.05);//animated
        float dt = (tmm.y - tmm.x) / stepsNum;   
        vec3 transmittance = pre_transmittance.xyz;
        vec3 scatteredLight = pre_scat.xyz;
        vec3 sigmaS = vec3(0.0);
        vec3 sigmaE = vec3(0.0);

        float pre_t = length(pre_pos.xyz - ro);
        t = max(pre_t, t);

        stepsNum *= TEMPORAL_ACCELERATION_MUL;
        stepsNum = t >= tmm.y ? 0 : stepsNum;

        for( int i = 0; i < stepsNum; ++i )
        {
            /*
            if(t > tmm.y)
            { 
                break;
            }*/

            vec3 p = ro + t * rd;
            float density = getDensity(p - YuShiPos);
            getParticipatingMedia(sigmaS, sigmaE, p);

            vec3 S = evaluateLightWithPhase(p) * sigmaS * density;
            sigmaE = max_f_v3(0.00001, sigmaE * density);
            vec3 sigmaE_INV = vec3(1.0 / sigmaE.x, 1.0 / sigmaE.y, 1.0 / sigmaE.z); 
            vec3 Sint = (S - S * exp(-sigmaE * dt)) * sigmaE_INV;
            scatteredLight += transmittance * Sint; // accumulate and also take into account the transmittance from previous steps
            transmittance *= exp(-sigmaE * dt);
                    
            t += dt;

        }
        
        pre_pos = vec4(ro + t * rd, pre_pos.w);
        pre_scat = vec4(scatteredLight, 0.0);
        pre_transmittance = vec4(transmittance, 0.0);
        return vec3(0.0);
    }
}

vec3 GetColor2(in float ID, in vec3 ro, in vec3 rd)
{
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    return WhiteWallColor;
}


bool HistoryRejection(in vec3 N1)
{
    float FOVEA_Factor = 1.0 - Map_HSV(HSV_PDF(degrees(acos(dot(N1, view_center_dir))))); /* 值越小，越触发 */
    const float Const_Factor = 0.05;
    //return F_ANG > 1.01;
    //return F_ANG + (FOVEA_Factor * Const_Factor)  > 1.04;
    // F_ANG / (FOVEA_Factor * Const_Factor) < 1.0;
    return F_DIS > 1.14 || F_ANG > 1.005;
}

void main()
{
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * (vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0) * far_plane);
    vec3 dir  = normalize(rd.xyz - orig);
    float W = rd.w;

    eyeAngle = degrees(acos(dot(dir, view_center_dir)));

    // Read history
    vec4 pos_tex = vec4(orig, 0.0);
    vec4 scat_tex = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 trans_tex = vec4(1.0, 1.0, 1.0, 1.0);
    //vec4 flags = vec4(1e-5);

    // Ray Marching
    vec2 res = RayCast(orig, dir);
    vec2 res2 = RayCast2(orig, dir);
    if(res.x < 0.0001) res.x = far_plane;
    vec3 pos = orig + dir * res.x;
    vec3 pos2 = orig + dir * res2.x;
    vec3 N = calcNormal(pos2);

     // Read History Data
    vec3 M_pos = pos;
    vec4 M_ndc = projection_pre * view_pre * vec4(M_pos, 1.0);
    M_ndc = M_ndc / M_ndc.w;
    vec2 M_uv = M_ndc.xy * 0.5 + 0.5;
    vec3 M_dir = normalize(M_pos - LastViewPosWS);

    // Solve History Data
//#if BASIC_ANIMATED_MEDIA == 0
    bool isFirstFrame = FrameIndex == 0;
    pos_tex = mix(texture(FinalPos_In, M_uv), vec4(orig, 0.0), isFirstFrame);
    scat_tex = mix(texture(ScatterLight_In, M_uv), vec4(0.0, 0.0, 0.0, 1.0), isFirstFrame);
    trans_tex = mix(texture(Transmittance_In, M_uv), vec4(1.0, 1.0, 1.0, 1.0), isFirstFrame);
    
    bool isOutRange = inRange(M_uv) < -0.05;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isOutRange);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 1.0), isOutRange);
    trans_tex = mix(trans_tex, vec4(1.0, 1.0, 1.0, 1.0), isOutRange);

    bool isReStart = HistoryRejection(dir);
    float blend_op = isReStart ? 0.6 : 0.0;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isReStart);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 1.0), blend_op);
    trans_tex = mix(trans_tex, vec4(1.0, 1.0, 1.0, 1.0), blend_op);

    bool isRefresh = FrameIndex % 200 == 0;
    pos_tex = mix(pos_tex, vec4(orig, 0.0), isRefresh);
    scat_tex = mix(scat_tex, vec4(0.0, 0.0, 0.0, 1.0), isRefresh);
    trans_tex = mix(trans_tex, vec4(1.0, 1.0, 1.0, 1.0), isRefresh);
//#endif
    // Compute ScatterLight
    vec3 baseColor = GetColor(res.y, orig, dir, pos_tex, scat_tex, trans_tex);
    vec3 baseColor2 = GetColor2(res2.y, orig, dir);

    // Light Compute
    vec3 diffuse_color = BaseColorReMap(baseColor2, WALL_METALLIC);
	float r = RoughnessReMap(WALL_ROUGHNESS);
	vec3 f0 = GetF0_All(baseColor, WALL_METALLIC, WALL_REFLECTANCE);
    vec3 result = vec3(0.0);
    for(int i = 0; i < pointLightsNum; ++i)
	{
		//light_Intensity * light_color * BRDF * NoL 
		vec3 L = normalize(pointLights[i].position - pos2);
		float NoL = clamp(dot(N, L), 0.001, 1.0);
		result = result + GetPointLightColor(i) * GetPointLightIllumiance(i, pos2)
		* Standard_BRDF_Torrance_DVF(diffuse_color, dir, L, N, r, f0, 1.0) 
        * NoL * volumetricShadow(pos2, pointLights[i].position);
	}	

    result = scat_tex.xyz + result *  length(trans_tex);
    // result = scat_tex.xyz + result *  max_v3_elem(trans_tex.xyz);

    // Show Light
    for(int i = 0; i < pointLightsNum; ++i)
	{
        vec2 re = iSphere(orig, dir, vec4(pointLights[i].position, LIGHT_SIZE));
        bool isLight = re.x < re. y && re.y > 0.0;
        result = mix(result, LIGHT_COLOR, isLight);
    }

    // Color Solve
    result = result/ (result + vec3(1.0));
    result = GammaCorrection(result);

    // Output
    FragColor = vec4(result, 1.0);
    ScatterLight_Out = scat_tex;
    Transmittance_Out = trans_tex;
    FinalPos_Out = pos_tex;
}


