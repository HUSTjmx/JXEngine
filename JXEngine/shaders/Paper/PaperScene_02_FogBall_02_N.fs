#version 330 core
layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 view_pre;
uniform mat4 projection;
uniform mat4 projection_inv;
uniform mat4 projection_pre;
uniform int IsFirstFrame;

// Fog Material Settings
#define SCATTERING  (0.7 * vec3(0.95, 0.5, 0.0))
#define ABSORPTION  (0.0 * vec3(0.75, 0.5, 0.0))
//0, 1 or 2
#define BASIC_ANIMATED_MEDIA 1

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
    // Change 1
    const vec2 YushiRadius = vec2(1.0, 0.5);
    const float rotateSpeed = 0.3;

// Wall Material
#define WALL_METALLIC 0.0
#define WALL_REFLECTANCE 0.1
#define WALL_ROUGHNESS 0.9

// Shade Settings
#define ShadowIntensity 3.0

// Phase Func Mode
#define PHASE_MODE 2.0
#define PHASE_G 0.5

float rand(vec3 co)
{
    return -1.0 + fract(sin(dot(co.xy,vec2(12.9898 + co.z,78.233))) * 43758.5453) * 2.0;
}

float HSV_PDF(float e)
{
    const float M = 1.0;
    if(e <= 5.79)return 1.0 * M * 1.0;
    return 7.49 / pow(0.3 * e + 1.0, 2.0) * M;
}

float Map_HSV(float v)
{
    return exp(1.0 - v) - 1.0;
}

vec3 StretchToFarPLane(in vec3 pos)
{
    vec3 v = normalize(pos - LastViewPosWS);
    float cos_theta = dot(normalize(Last_view_center_dir), v);
    float len = far_plane / cos_theta;
    //return vec3(far_plane / len);
    return LastViewPosWS + v * vec3(len);
}



float PhaseFunc(vec3 V, vec3 L, float g)
{
    float mu = dot(V, L);
    if(PHASE_MODE < 0.5) return PhaseFunc_CON();
    if(PHASE_MODE < 1.5) return PhaseFunc_RAYLEIGH(mu);
    if(PHASE_MODE < 2.5) return PhaseFunc_MIE(mu, g);
    return PhaseFunc_CON();
}

float sdPlane(vec3 p, vec3 n /* normalized */, float h)
{
    return dot(p, n) + h;
}

// Change 2
float sdTorus(vec3 p, vec3 center, vec2 t)
{
  p = p - center;
  vec2 q = vec2(length(p.xy) - t.x, p.z);
  return length(q) - t.y;
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
    // Change 3
    res = opU(res, vec2(sdSphere(pos, YuShiPos, YushiRadius.x + 2.5), OBJ_YUSHI_SPHERE));

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

// Change 4
void getParticipatingMedia(out vec3 sigmaS, out vec3 sigmaE, in vec3 pos)
{
    pos = (pos - YuShiPos) * rotate_around_y(180 * sin(iTime * rotateSpeed)) + YuShiPos;
    float sphereFog = clamp(sdTorus(pos, YuShiPos, YushiRadius) * -3.2, 0.0, 1.0);
    
    sigmaS = sphereFog * SCATTERING;
    sigmaE = ABSORPTION + sigmaS;

#if BASIC_ANIMATED_MEDIA==1
    float r = floor(iTime);
    sigmaS = sphereFog * abs(5.0* vec3(rand(vec3(r,0.0,1.0)),rand(vec3(r,0.0,5.0)),rand(vec3(r,0.0,9.0))));
    vec3 absorption = abs(0.1* vec3(rand(vec3(r,1.0,2.0)),rand(vec3(r,1.0,7.0)),rand(vec3(r,1.0,7.0))));
    sigmaE = sigmaS + absorption;
#elif BASIC_ANIMATED_MEDIA==2
    float r = iTime*0.2;
    sigmaS = sphereFog * abs(5.0* vec3(sin(r*1.1),sin(r*3.3),sin(r*5.5)));
    vec3 absorption = abs( 0.1* vec3(sin(r*2.2),sin(r*4.4),sin(r*6.6)));
    sigmaE = sigmaS + absorption;
#endif

    sigmaE = max_f_v3(0.0000001, sigmaE);
}

float volumetricShadow(in vec3 from, in vec3 to)
{
    //return 2.0;
    const float numStep = 36.0; // quality control. Bump to avoid shadow alisaing
    vec3 shadow = vec3(1.0);
    vec3 sigmaS = vec3(0.0);
    vec3 sigmaE = vec3(0.0);
    float dd = length(to-from) / numStep;
    for(float s = 0.5; s < (numStep - 0.1); s += 1.0)// start at 0.5 to sample at center of integral part
    {
        vec3 pos = from + (to - from) * (s / (numStep));
        getParticipatingMedia(sigmaS, sigmaE, pos);
        shadow = shadow * exp(-sigmaE * ShadowIntensity * dd);
    }
    return length(shadow) * 0.8;
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
    pre_scat = vec4(0.0, 0.0, 0.0, 1.0);
    pre_transmittance = vec4(1.0, 1.0, 1.0, 0.0);
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    else
    {
        // Change 5
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, max(YushiRadius.x, YushiRadius.y) + 1.0));
        float t = tmm.x;
        float dt = .5;     //float dt = .2 - .195*cos(iTime*.05);//animated
        vec3 transmittance = vec3(1.0, 1.0, 1.0);
        vec3 scatteredLight = vec3(0.0);
        vec3 sigmaS = vec3(0.0);
        vec3 sigmaE = vec3(0.0);

        
        for( int i = 0; i < 10; ++i )
        {
            vec3 p = ro + t * rd;
            getParticipatingMedia(sigmaS, sigmaE, p);

            vec3 S = evaluateLightWithPhase(p) * sigmaS;
            vec3 Sint = (S - S * exp(-sigmaE * dt)) / sigmaE;
            scatteredLight += transmittance * Sint; // accumulate and also take into account the transmittance from previous steps
            transmittance *= exp(-sigmaE * dt);
                    
            t += dt;
            if(t > tmm.y)
            { 
                pre_pos.w = 1.0;
                break;
            }
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

void main()
{
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * (vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0) * far_plane);
    vec3 dir  = normalize(rd.xyz - orig);
    float W = rd.w;

    // Read history
    vec4 pos_tex;
    vec4 scat_tex;
    vec4 trans_tex;
    //vec4 flags = vec4(1e-5);

    // Ray Marching
    vec2 res = RayCast(orig, dir);
    vec2 res2 = RayCast2(orig, dir);
    if(res.x < 0.0001) res.x = far_plane;
    vec3 pos = orig + dir * res.x;
    vec3 pos2 = orig + dir * res2.x;
    vec3 N = calcNormal(pos2);

    vec3 baseColor = GetColor(res.y, orig, dir, pos_tex, scat_tex, trans_tex);
    vec3 baseColor2 = GetColor2(res2.y, orig, dir);

    // Light Compute
    vec3 diffuse_color = BaseColorReMap(baseColor2, WALL_METALLIC);
	float r = RoughnessReMap(WALL_ROUGHNESS);
	vec3 f0 = GetF0_All(baseColor, WALL_METALLIC, WALL_REFLECTANCE);

    vec3 result = vec3(0.0);
    float sd;
    for(int i = 0; i < pointLightsNum; ++i)
	{
        sd = volumetricShadow(pos2, pointLights[i].position);
		//light_Intensity * light_color * BRDF * NoL 
		vec3 L = normalize(pointLights[i].position - pos2);
		float NoL = clamp(dot(N, L), 0.001, 1.0);
		result = result + GetPointLightColor(i) * GetPointLightIllumiance(i, pos2)
		* Standard_BRDF_Torrance_DVF(diffuse_color, dir, L, N, r, f0, 1.0) 
        * NoL * volumetricShadow(pos2, pointLights[i].position);
	}	

    //result += (vec3(0.1, 0.1, 0.1) * GetPointLightColor(0) * diffuse_color);
    //if(res.y > 3.9)
    result = scat_tex.xyz + result *  length(trans_tex);
    // result = scat_tex.xyz + result *  max_v3_elem(trans_tex.xyz);

    // Show Light
    for(int i = 0; i < pointLightsNum; ++i)
	{
        vec2 re = iSphere(orig, dir, vec4(pointLights[i].position, LIGHT_SIZE));
        bool isLight = re.x < re. y && re.y > 0.0;
        result = mix(result, LIGHT_COLOR, isLight);
    }

    result = result/ (result + vec3(1.0));
    result = GammaCorrection(result);

    // Output
    FragColor = vec4(result, 1.0);
    
   // FragColor = vec4(scat_tex.xyz, 1.0);
   // FragColor = vec4(N, 1.0);
   //FragColor = vec4(NL, NL, NL, 1.0);
   //FragColor = vec4(sd, sd, sd, 1.0);
    // Test
    //if(IsFirstFrame % 100 < 50)
        //   FragColor = vec4(M_uv, 1.0, 1.0);
    //else FragColor = vec4(TexCoords, 1.0, 1.0);

    //FragColor = vec4(abs(M_uv - TexCoords) * 10.0, 0.0, 1.0);
    // FinalPos = vec4(pos, 1.0);
    // FragColor = texture(posTex, pre_uv);

    // FragColor = vec4(IsFirstFrame / 1000.0);

    // FragColor = vec4(baseColor, 1.0);

    //  FragColor = vec4(dir, 1.0);
    // FragColor = rd;
    //FragColor = vec4(rd.www * 10.0, 1.0);

    //FragColor = vec4(abs(Last_view_center_dir), 1.0);

    // FragColor = vec4(StretchToFarPLane(orig + dir), 1.0);
    
    //if(inRange(M_uv) < 0.000001) FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    //if(HistoryRejection(dir, M_dir) < 0.0001) FragColor = vec4(1.0, 1.0, 0.0, 1.0);

    //FragColor = vec4(vec3(dot(M_dir, dir)), 1.0);
}

