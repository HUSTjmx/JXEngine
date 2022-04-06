#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 LastColor;
layout(location = 2) out vec4 ScatterLight;
layout(location = 3) out vec4 FinalPos;

in vec2 TexCoords;

uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 view_pre;
uniform mat4 projection;
uniform mat4 projection_inv;
uniform mat4 projection_pre;
uniform sampler2D preTexture;
uniform sampler2D scatterTex;
uniform sampler2D posTex;
uniform int IsFirstFrame;

#define float2 vec2
#define float3 vec3
#define float4 vec4
#define uint2 uvec2
#define uint3 uvec3
#define uint4 uvec4

// Box
#define BOX_SIZE vec3(4.0, 4.0, 4.0)
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
#define OBJ_BUNNY 4.0
    const vec3 BunnyPos = vec3(0.0, 0.0, 0.0);
    const vec3 BunnyBOX = vec3(1.0, 1.0, 1.0); 

// Wall Material
#define WALL_METALLIC 0.0
#define WALL_REFLECTANCE 0.1
#define WALL_ROUGHNESS 0.9

// History Rejection
#define HISTORY_REJECTION_COS_MIN 0.99
#define HISTORY_REJECTION_DIS_MAX 1
#define DIS_WEIGHT 0.4
#define ANGLE_WEIGHT (1.0 - DIS_WEIGHT)
#define FOVEA_CONST_FACTOR 0.5

// Phase Func Mode
#define PHASE_MODE 2.0
#define PHASE_G 0.5

////////// Parameters

// Participating media properties. have fun tweaking those :)
float3 scattering = 25.0 * float3(0.25,0.5,1.0);
float3 absorption = 0.0 * float3(0.75,0.5,0.0);
#define extinction  (absorption + scattering)
// Default noise erosion strength
float erosionStrength = 1.0;

////////// Options

#define VOLUME_FILTERING_NEAREST 0

#define FBM_NOISE 0

#define BASIC_ANIMATED_NOISE 1

//0, 1 or 2
#define BASIC_ANIMATED_MEDIA 0

float PhaseFunc_UNIFORM()
{
    return 0.25 * PI_INV;
}

float PhaseFunc_RAYLEIGH(float mu)
{
    float A = 3.0 / 16.0 * PI_INV;
    return A * (1.0 + mu * mu);
}

float PhaseFunc_MIE(float mu, float g)
{
    float A = 3.0 / 8.0 * PI_INV;
    float g2 = g * g;
    float mu2 = mu * mu;

    float numerator = (1.0 - g2) * (1.0 + mu2);

    float denominator = (2.0 + g2) * pow(1.0 + g2 - 2.0 * g * mu, 1.50);

    return A * numerator / denominator;
}

float PhaseFunc(vec3 V, vec3 L, float g)
{
    float mu = dot(V, L);
    if(PHASE_MODE < 0.5) return PhaseFunc_UNIFORM();
    if(PHASE_MODE < 1.5) return PhaseFunc_RAYLEIGH(mu);
    if(PHASE_MODE < 2.5) return PhaseFunc_MIE(mu, g);
    return PhaseFunc_UNIFORM();
}

//////////////////////////////////////////////////
// Bunny volume data
//////////////////////////////////////////////////

// Packed 32^3 bunny data as 32x32 uint where each bit represents density per voxel
#define BUNNY_VOLUME_SIZE 32
const uint packedBunny[1024] = uint[1024](0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,917504u,917504u,917504u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,1966080u,12531712u,16742400u,16742400u,16723968u,16711680u,8323072u,4128768u,2031616u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,6144u,2063360u,16776704u,33553920u,33553920u,33553920u,33553920u,33520640u,16711680u,8323072u,8323072u,2031616u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,268435456u,402653184u,134217728u,201326592u,67108864u,0u,0u,7168u,2031104u,16776960u,33554176u,33554176u,33554304u,33554176u,33554176u,33554176u,33553920u,16744448u,8323072u,4128768u,1572864u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,805306368u,939524096u,402653184u,478150656u,260046848u,260046848u,260046848u,125832192u,130055680u,67108608u,33554304u,33554304u,33554304u,33554304u,33554304u,33554304u,33554304u,33554176u,16776704u,8355840u,4128768u,917504u,0u,0u,0u,0u,0u,0u,0u,0u,0u,805306368u,1056964608u,1056964608u,528482304u,528482304u,260046848u,260046848u,260046848u,130039296u,130154240u,67108739u,67108807u,33554375u,33554375u,33554370u,33554368u,33554368u,33554304u,33554304u,16776960u,8330240u,4128768u,393216u,0u,0u,0u,0u,0u,0u,0u,0u,939524096u,1040187392u,1040187392u,520093696u,251658240u,251658240u,260046848u,125829120u,125829120u,130088704u,63045504u,33554375u,33554375u,33554375u,33554407u,33554407u,33554370u,33554370u,33554374u,33554310u,16776966u,4144642u,917504u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,15360u,130816u,262017u,4194247u,33554383u,67108847u,33554415u,33554407u,33554407u,33554375u,33554375u,33554318u,2031502u,32262u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,31744u,130816u,262019u,2097151u,134217727u,134217727u,67108863u,33554415u,33554407u,33554415u,33554383u,2097102u,982926u,32262u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,31744u,130816u,524263u,117964799u,127926271u,134217727u,67108863u,16777215u,4194303u,4194303u,2097151u,1048574u,65422u,16134u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3u,31751u,130951u,524287u,252182527u,261095423u,261095423u,59768830u,2097150u,1048574u,1048575u,262143u,131070u,65534u,16134u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,7u,31751u,130959u,503840767u,520617982u,529530879u,261095423u,1048575u,1048574u,1048574u,524286u,524287u,131070u,65534u,16134u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3u,1799u,32527u,134348750u,1040449534u,1057488894u,520617982u,51380223u,1048575u,1048575u,524287u,524287u,524287u,131070u,65534u,15886u,6u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,1536u,3968u,8175u,65535u,1006764030u,1040449534u,1057488894u,50855934u,524286u,524286u,524287u,524287u,524286u,262142u,131070u,65534u,32270u,14u,6u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3968u,8160u,8191u,805371903u,2080505854u,2114191358u,101187582u,34078718u,524286u,524286u,524286u,524286u,524286u,524286u,262142u,131070u,32766u,8078u,3590u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8128u,8176u,16383u,2013331455u,2080505854u,235143166u,101187582u,524286u,1048574u,1048574u,1048574u,1048574u,524286u,524286u,262142u,131070u,32766u,16382u,8070u,1024u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8160u,8184u,1879064574u,2013331455u,470024190u,67371006u,524286u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,524286u,262142u,65534u,16382u,8160u,1024u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8128u,8184u,805322750u,402718719u,134479870u,524286u,524286u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,65534u,16382u,16368u,1792u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3968u,8184u,16382u,131071u,262142u,524286u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,65534u,16382u,16368u,1792u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,1792u,8184u,16380u,65535u,262143u,524286u,524286u,1048574u,1048574u,1048575u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,65534u,16376u,16368u,1792u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8176u,16376u,32767u,262143u,524286u,1048574u,1048574u,1048575u,1048575u,1048575u,1048575u,1048574u,1048574u,524286u,262142u,32766u,16376u,8176u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,4032u,8184u,32766u,262142u,524286u,524286u,1048575u,1048574u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,262142u,32766u,16376u,8176u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,384u,8184u,32766u,131070u,262142u,524286u,1048575u,1048574u,1048574u,1048574u,1048574u,1048574u,524286u,524286u,131070u,32766u,16368u,1920u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,4080u,32764u,65534u,262142u,524286u,524286u,524286u,1048574u,1048574u,524286u,524286u,524286u,262142u,131070u,32764u,8160u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,256u,16376u,32760u,131068u,262140u,262142u,524286u,524286u,524286u,524286u,524286u,262142u,131070u,65532u,16368u,3840u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3968u,32752u,65528u,131068u,262142u,262142u,262142u,262142u,262142u,262142u,262140u,131064u,32752u,7936u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,8064u,32736u,65528u,131070u,131070u,131070u,131070u,131070u,131070u,65532u,32752u,8160u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,3456u,16376u,32764u,65534u,65534u,65534u,32766u,32764u,16380u,4048u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,48u,2680u,8188u,8188u,8188u,8188u,4092u,120u,16u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,120u,248u,508u,508u,508u,248u,240u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,96u,240u,504u,504u,504u,240u,96u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,224u,224u,224u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u);


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

//////////////////////////////////////////////////
// Volume noise from somewhere...
//////////////////////////////////////////////////

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


//////////////////////////////////////////////////
// Cube intersection
//////////////////////////////////////////////////

float3 worldPosTocubePos(float3 worldPos)
{
    // cube of world space size 4 with bottom face on the ground y=0
    return worldPos*0.15 + float3(0.0,-0.5,0.0);
}

// From https://www.shadertoy.com/view/4s23DR
bool cube(vec3 org, vec3 dir, out float near, out float far)
{
	// compute intersection of ray with all six bbox planes
	vec3 invR = 1.0/dir;
	vec3 tbot = invR * (-0.5 - org);
	vec3 ttop = invR * (0.5 - org);
	
	// re-order intersections to find smallest and largest on each axis
	vec3 tmin = min (ttop, tbot);
	vec3 tmax = max (ttop, tbot);
	
	// find the largest tmin and the smallest tmax
	vec2 t0 = max(tmin.xx, tmin.yz);
	near = max(t0.x, t0.y);
	t0 = min(tmax.xx, tmax.yz);
	far = min(t0.x, t0.y);

	// check for hit
	return near < far && far > 0.0;
}

// Get density for a position
float getDensity(float3 cubePos)
{
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

// Returns the light distance attenuation
float distanceAttenuation(float distance)
{
    float lightMaxRadius = 3.0;
    float linAtt = clamp((lightMaxRadius-distance)/lightMaxRadius,0.0,1.0);
    linAtt*=linAtt;	// some "fake artistic" attenuation
    return linAtt/(distance*distance);
}


float sdPlane(vec3 p, vec3 n /* normalized */, float h)
{
    return dot(p, n) + h;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

vec2 iBox(in vec3 ro, in vec3 rd, in vec3 rad)
{
    vec3 m = 1.0 / rd;
    vec3 n = m * ro;
    vec3 k = abs(m) * rad;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return vec2(-1.0);
    return vec2( tN, tF );
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
    res = opU(res, vec2(sdBox(pos - BunnyPos, BunnyBOX), OBJ_BUNNY));

    return res;
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

vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0) * 0.5773 * 0.0005;
    return normalize( e.xyy * Map( pos + e.xyy ).x + 
					  e.yyx * Map( pos + e.yyx ).x + 
					  e.yxy * Map( pos + e.yxy ).x + 
					  e.xxx * Map( pos + e.xxx ).x );
}

// Get transmittance from a direction and distance onto a point (volume shadows)
float3 volumetricShadow(float3 cubePos, float3 Lpos)
{
    return vec3(1.0);
    float3 shadow = float3(1.0);
    float3 Ldir = normalize(Lpos - cubePos);
    float sampledDistance = 0.1;
    float stepSizeShadow = 0.1;
    for(float tshadow = 0.0; tshadow < sampledDistance; tshadow += stepSizeShadow)
    {
        float3 cubeShadowPos = cubePos + tshadow * Ldir;
        float densityShadow = getDensity(cubeShadowPos);
        shadow *= exp(-densityShadow * extinction * stepSizeShadow);
    }
    return shadow;
}

vec3 evaluateLightWithPhase(in vec3 pos)
{
    vec3 light_sum = vec3(0.0);
    vec3 V = normalize(pos - viewPosWS);
    for(int i = 0; i < pointLightsNum; ++i)
        light_sum += GetPointLightIllumiance(i, pos) * PhaseFunc(V, normalize(pointLights[i].position - viewPosWS), PHASE_G) * volumetricShadow(pos, pointLights[i].position);
    
    for(int i = 0; i < spotLightsNum; ++i)
        light_sum += GetSpotLightIllumiance(i, pos) * PhaseFunc(V, normalize(spotLights[i].position - viewPosWS), PHASE_G) * volumetricShadow(pos, spotLights[i].position);

    for(int i = 0; i < dirLightsNum; ++i)
        light_sum += GetDirLightIllumiance(i) * PhaseFunc(V, dirLights[i].direction, PHASE_G);
    
    return light_sum;
}

vec3 GetColor(in float ID, in vec3 ro, in vec3 rd, in vec3 sigmaS, in vec3 sigmaE, inout vec4 pre_pos, inout vec4 pre_scat, in vec4 flags)
{
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    if(ID < 4.5)
    {
        vec2 tmm = iBox(ro, rd, BunnyBOX);
        float t = tmm.x;
        float dt = .01;     //float dt = .2 - .195*cos(iTime*.05);//animated
        vec3 transmittance = vec3(1.0);
        vec3 scatteredLight = vec3(0.0);
    
        for(int i = 0; i < 1; ++i)
        {
            float3 rayPos = ro + t * rd;
            float density = getDensity(worldPosTocubePos(rayPos));


            vec3 S = evaluateLightWithPhase(rayPos) * sigmaS * density;
            vec3 m_sigmaE = max(vec3(0.0001), density * sigmaE);
            vec3 Sint = (S - S * exp(-m_sigmaE * dt)) / m_sigmaE;
            scatteredLight += transmittance * Sint; // accumulate and also take into account the transmittance from previous steps
            transmittance *= exp(-m_sigmaE * dt);
                    
            t += dt;
            if(t > tmm.y)
            { 
                break;
            }
        }
        pre_pos = vec4(ro + t * rd, pre_pos.w);
        pre_scat = vec4(scatteredLight, transmittance);
    }
    return vec3(0.0);
}


void main()
{
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * (vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0) * far_plane);
    vec3 dir  = normalize(rd.xyz - orig);
    float W = rd.w;

     // Read history
    vec4 pos_tex;
    vec4 scat_tex;
    vec4 flags = vec4(1e-5);

    // Ray Marching
    vec2 res = RayCast(orig, dir);
    if(res.x < 0.0001) res.x = far_plane;
    vec3 pos = orig + dir * res.x;
    vec3 N = calcNormal(pos);

    // 散射属性的设置
#if BASIC_ANIMATED_MEDIA==1
    float r = floor(time);
    scattering = abs(25.0* float3(rand(float3(r,0.0,1.0)),rand(float3(r,0.0,5.0)),rand(float3(r,0.0,9.0))));
    absorption = abs(5.0* float3(rand(float3(r,1.0,2.0)),rand(float3(r,1.0,7.0)),rand(float3(r,1.0,7.0))));
#elif BASIC_ANIMATED_MEDIA==2
    float r = time*0.2;
    scattering = abs(25.0* float3(sin(r*1.1),sin(r*3.3),sin(r*5.5)));
    absorption = abs( 5.0* float3(sin(r*2.2),sin(r*4.4),sin(r*6.6)));
#endif

    vec3 baseColor = GetColor(res.y, orig, dir, scattering, scattering + absorption, pos_tex, scat_tex, flags);

    // Light Compute
    vec3 diffuse_color = BaseColorReMap(baseColor, WALL_METALLIC);
	float r = RoughnessReMap(WALL_ROUGHNESS);
	vec3 f0 = GetF0_All(baseColor, WALL_METALLIC, WALL_REFLECTANCE);

    vec3 result = vec3(0.0);
    for(int i = 0; i < pointLightsNum; ++i)
	{
		//light_Intensity * light_color * BRDF * NoL 
		vec3 L = normalize(pointLights[i].position - pos);
		float NoL = clamp(dot(N, L), 0.0, 1.0);
		result = result + GetPointLightColor(i) * GetPointLightIllumiance(i, pos)
		* Standard_BRDF_Torrance_DVF(diffuse_color, dir, L, N, r, f0, 1.0) * NoL; 
	}	

    //result += (vec3(0.1, 0.1, 0.1) * GetPointLightColor(0) * diffuse_color);
    if(res.y > 3.9)
        result += scat_tex.xyz;
    result = result/ (result + vec3(1.0));
    result = GammaCorrection(result);

    // Output
    FragColor = vec4(result, 1.0);
    FinalPos = pos_tex;
    ScatterLight = scat_tex;
}