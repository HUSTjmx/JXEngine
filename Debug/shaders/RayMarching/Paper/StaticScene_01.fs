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
#define OBJ_YUSHI_SPHERE 4.0
    const vec3 YuShiPos = vec3(0.0, 0.0, 0.5);
    const float YushiRadius = 2.0; 

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

#define MAX_FACTOR 1.0

// Phase Func Mode
#define PHASE_MODE 2.0
#define PHASE_G 0.5

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

vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0) * 0.5773 * 0.0005;
    return normalize( e.xyy * Map( pos + e.xyy ).x + 
					  e.yyx * Map( pos + e.yyx ).x + 
					  e.yxy * Map( pos + e.yxy ).x + 
					  e.xxx * Map( pos + e.xxx ).x );
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

void getParticipatingMedia(out float sigmaS, out float sigmaE, in vec3 pos)
{
    float sphereFog = clamp((YushiRadius - length(pos - YuShiPos))/YushiRadius, 0.0, 1.0);
    
    sigmaS = sphereFog * 0.1;
    const float sigmaA = 0.00;
    sigmaE = max(0.0000001, sigmaA + sigmaS);
}

float volumetricShadow(in vec3 from, in vec3 to)
{
    return 1.0;
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


vec3 GetColor(in float ID, in vec3 ro, in vec3 rd, inout vec4 pre_pos, inout vec4 pre_scat, in vec4 flags)
{
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    if(ID < 4.5)
    {
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, YushiRadius));
        float t = tmm.x;
        float dt = .05;     //float dt = .2 - .195*cos(iTime*.05);//animated
        float transmittance = 1.0;
        vec3 scatteredLight = vec3(0.0);
        float sigmaS = 0.0;
        float sigmaE = 0.0;

        float pre_t = length(pre_pos.xyz - ro);

        if(IsFirstFrame > 0 && length(pre_scat.xyz) > 0.00001)
        {
            scatteredLight = pre_scat.xyz;
            t = max(pre_t, t);
            transmittance = pre_scat.w;
        }

        if(pre_pos.w < 0.5)
        {
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
        }
        else
        {
            pre_pos.w = 1.0;
        }

        
        pre_pos = vec4(ro + t * rd, pre_pos.w);
        pre_scat = vec4(scatteredLight, transmittance);
        //if(IsFirstFrame % 50 < 25)
        //{
        //    return normalize(pre_pos.xyz - ro);
        //}
        //else{
        //    return rd;
        //}
        
        //return vec3(text_);
        //return pre_t;
        return vec3(0.0);
    }
    return vec3(0.0);
}

float HistoryRejection(in vec3 N1, in vec3 N2)
{
    // 颜色无法考虑，因为我们不知道当前帧的颜色
    // 距离因子：主要是由于移动导致的采样误差，以及突然的遮挡和出现。
    float Dis_Factor = length(LastViewPosWS - viewPosWS);  /* 值越大，越触发 */
    Dis_Factor = clamp(Dis_Factor / HISTORY_REJECTION_DIS_MAX, 0.0, 1.0);
    float Angle_Factor = dot(normalize(view_center_dir), normalize(Last_view_center_dir));  /* 值越小，越触发 */
    Angle_Factor = 1.0 - Angle_Factor;
    float FOVEA_Factor = Map_HSV(HSV_PDF(degrees(acos(dot(N1, view_center_dir))))); /* 值越小，越触发 */
    const float Const_Factor = 0.1;

    float factor = (Dis_Factor * DIS_WEIGHT + Angle_Factor * ANGLE_WEIGHT) / (FOVEA_CONST_FACTOR + FOVEA_Factor);

    
    if(Angle_Factor > 0.01)
        return -1.0;
    return 1.0;
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

    // Read Pre Data
    vec3 M_pos = pos;
    vec4 M_ndc = projection_pre * view_pre * vec4(M_pos, 1.0);
    M_ndc = M_ndc / M_ndc.w;
    vec2 M_uv = M_ndc.xy * 0.5 + 0.5;
    vec3 M_dir = normalize(M_pos - LastViewPosWS);

    if(IsFirstFrame == 0)
    {
        pos_tex = vec4(orig, 0.0);
        scat_tex = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        pos_tex = texture(posTex, M_uv);
        scat_tex = texture(scatterTex, M_uv);
    }

    if(HistoryRejection(dir, M_dir) < 0.001)
    {
        pos_tex = vec4(orig, 0.0);
        scat_tex = vec4(0.0, 0.0, 0.0, 1.0);
    }

    if(inRange(M_uv) < -0.05)
    {
        pos_tex = vec4(orig, 0.0);
        scat_tex = vec4(0.0, 0.0, 0.0, 1.0);
    }

   

    vec3 baseColor = GetColor(res.y, orig, dir, pos_tex, scat_tex, flags);

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

    result += (vec3(0.1, 0.1, 0.1) * GetPointLightColor(0) * diffuse_color);
    if(res.y > 3.9)
        result += scat_tex.xyz;
    result = result/ (result + vec3(1.0));
    result = GammaCorrection(result);

    // Output
    FragColor = vec4(result, 1.0);
    FinalPos = pos_tex;
    ScatterLight = scat_tex;
    
    // Test
    //if(IsFirstFrame % 100 < 50)
        //   FragColor = vec4(M_uv, 1.0, 1.0);
    //else FragColor = vec4(TexCoords, 1.0, 1.0);

    //FragColor = vec4(abs(M_uv - TexCoords) * 10.0, 0.0, 1.0);
    // FinalPos = vec4(pos, 1.0);
    // FragColor = texture(posTex, pre_uv);

    // FragColor = vec4(IsFirstFrame / 1000.0);

    // FragColor = vec4(baseColor, 1.0);

    //   FragColor = vec4(dir, 1.0);
    // FragColor = rd;
    //FragColor = vec4(rd.www * 10.0, 1.0);

    //FragColor = vec4(abs(Last_view_center_dir), 1.0);

    // FragColor = vec4(StretchToFarPLane(orig + dir), 1.0);
    
    //if(inRange(M_uv) < 0.000001) FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    //if(HistoryRejection(dir, M_dir) < 0.0001) FragColor = vec4(1.0, 1.0, 0.0, 1.0);

    FragColor = vec4(vec3(dot(M_dir, dir)), 1.0);
}


/*

if(IsFirstFrame > 0)
        {
            col = pre_scat.xyz;
            t = max(pre_t, t);
            c = pre_scat.w;
        }

        if(pre_pos.w < 0.5)
        {
            for( int i = 0; i < 10; ++i )
            {
                t += dt * exp(-2. * c);
                if(t > tmm.y)
                { 
                    pre_pos.w = 1.0;
                    break;
                }
                        
                c = GetYushiColor(ro + t * rd);               
                
                // col = .99 * col + .08 * vec3(c * c, c * c * c, c);//green	
                col = .9*col+ .08*vec3(c*c*c, c*c, c);//blue
            }
        }
        else
        {
            pre_pos.w = 1.0;
        }

        
        pre_pos = vec4(ro + t * rd, pre_pos.w);
        pre_scat = vec4(col, c);
        
*/






/*
if(IsFirstFrame > 0)
        {
            scatteredLight = pre_scat.xyz;
            t = max(pre_t, t);
            transmittance = pre_scat.w;
        }

        if(pre_pos.w < 0.5)
        {
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
        }
        else
        {
            pre_pos.w = 1.0;
        }

        
        pre_pos = vec4(ro + t * rd, pre_pos.w);
        pre_scat = vec4(scatteredLight, transmittance);
*/




/*

if(IsFirstFrame > 0)
        {
            scatteredLight = pre_scat.xyz;
            t = max(pre_t, t);
            transmittance = pre_scat.w;
        }

        if(pre_pos.w < 0.5)
        {
            for( int i = 0; i < 5; ++i )
            {
                t += dt * exp(-2. * transmittance);
                if(t > tmm.y)
                { 
                    pre_pos.w = 1.0;
                    break;
                }
                        
                transmittance = GetYushiColor(ro + t * rd);               
                
                // col = .99 * col + .08 * vec3(c * c, c * c * c, c);//green	
                scatteredLight = .9*scatteredLight+ .08*vec3(transmittance*transmittance*transmittance, transmittance*transmittance, transmittance);//blue
            }
        }
        else
        {
            pre_pos.w = 1.0;
        }

        
        pre_pos = vec4(ro + t * rd, pre_pos.w);
        pre_scat = vec4(scatteredLight, transmittance);
    
*/