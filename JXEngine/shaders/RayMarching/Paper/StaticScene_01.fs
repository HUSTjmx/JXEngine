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
    const float YushiRadius = 1.0; 

// Wall Material
#define WALL_METALLIC 0.0
#define WALL_REFLECTANCE 0.1
#define WALL_ROUGHNESS 0.9


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
        p.yz= csqr(p.yz);
        p = p.zxy;
        res += exp(-19. * abs(dot(p,c)));
        
	}
	return res / 2.;
}

vec3 GetColor(in float ID, in vec3 ro, in vec3 rd)
{
    if(ID < 0.5) return vec3(0.0);
    if(ID < 1.5) return WhiteWallColor;
    if(ID < 2.5) return RightWallColor;
    if(ID < 3.5) return LeftWallColor;
    if(ID < 4.5)
    {
        vec2 tmm = iSphere(ro, rd, vec4(YuShiPos, YushiRadius));
        float t = tmm.x;
        float dt = .02;
        //float dt = .2 - .195*cos(iTime*.05);//animated
        float c = 0.0;
        vec3 col = vec3(0.0);
        for( int i = 0; i < 40; ++i )
        {
            t += dt * exp(-2. * c);
            if(t > tmm.y)
            {
                t -= dt * exp(-2. * c);
                break;
            }
                    
            c = GetYushiColor(ro + t * rd);               
            
           // col = .99 * col + .08 * vec3(c * c, c * c * c, c);//green	
           col = .99*col+ .08*vec3(c*c*c, c*c, c);//blue
        }
        return col * 0.5;
    }
    return vec3(0.0);
}

void main()
{
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * vec4(vec3(ReMapNDC_V2(TexCoords), 1.0), 1.0);
    vec3 dir = normalize(rd.xyz);

    vec2 res = RayCast(orig, dir);
    vec3 pos = orig + dir * res.x;
    vec3 N = calcNormal(pos);
    vec3 result = vec3(0.0);

    vec3 baseColor = GetColor(res.y, orig, dir);

    vec3 diffuse_color = BaseColorReMap(baseColor, WALL_METALLIC);
	float r = RoughnessReMap(WALL_ROUGHNESS);
	vec3 f0 = GetF0_All(baseColor, WALL_METALLIC, WALL_REFLECTANCE);


    for(int i = 0; i < pointLightsNum; ++i)
	{
		//light_Intensity * light_color * BRDF * NoL 
		vec3 L = normalize(pointLights[i].position - pos);
		float NoL = clamp(dot(N, L), 0.0, 1.0);
		result = result + GetPointLightColor(i) * GetPointLightIllumiance(i, pos)
		* Standard_BRDF_Torrance_DVF(diffuse_color, dir, L, N, r, f0, 1.0) * NoL; 
	}	

    result += (vec3(0.04, 0.04, 0.04) * GetPointLightColor(0) * diffuse_color);


    FragColor = vec4(result, 1.0);


}