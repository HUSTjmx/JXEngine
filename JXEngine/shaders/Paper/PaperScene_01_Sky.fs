#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 LastColor;
layout(location = 2) out vec4 ScatterLight;
layout(location = 3) out vec4 FinalPos;

in vec2 TexCoords;

//uniform sampler2D noisyTexture;
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

vec3 computeIncidentLight_T(vec3 orig, vec3 dir, vec3 sunDirection, float tmin, float tmax)
{
    float t0 = 0.0, t1 = 0.0;
    if(!raySphereIntersect(orig, dir, atmosphereRadius, t0, t1) || t1 < 0.000001)return vec3(0.0);
    if(t0 > tmin && t0 > 0.000001) tmin = t0;
    if(t1 < tmax) tmax = t1;



    int numSamples = 16;
    int numSamplesLight = 8;


    float segmentLength = (tmax - tmin) / numSamples;
    float tCurrent = tmin;
    
    // mie and rayleigh contribution
    // ...
    vec3 sumR = vec3(0.0), sumM = vec3(0.0);

    float opticalDepthR = 0.0, opticalDepthM = 0.0;

    // mu in the paper which is the cosine of the angle between the sun direction and the ray direction.
    // ...
    float mu = dot(dir, sunDirection);

    float phaseR = PhaseFunc_RAYLEIGH(mu);
    float phaseM = PhaseFunc_MIE(mu, MIE_G);

    for(int i = 0; i < numSamples; ++i)
    {
        vec3 samplePosition = orig + (tCurrent + segmentLength * 0.5) * dir;
        float height = length(samplePosition) - earthRadius;

        // Compute optical depth for light
        // ...
        float hr = exp(-height / Hr) * segmentLength;
        float hm = exp(-height / Hm) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;

        // light optical depth.
        // ...
        float t0Light, t1Light;
        raySphereIntersect(samplePosition, sunDirection, atmosphereRadius, t0Light, t1Light);
        float segmentLengthLight = t1Light / numSamplesLight, tCurrentLight = 0.0;
        float opticalDepthLightR = 0.0, opticalDepthLightM = 0.0;
        int j;
        for(j = 0; j < numSamplesLight; ++j)
        {
            vec3 samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5) * sunDirection;
            float heightLight = length(samplePositionLight) - earthRadius;
            if(heightLight < 0.000001)break;
            opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
            opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }

        if(j == numSamplesLight)
        {
            vec3 tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1 * (opticalDepthLightM + opticalDepthM);
            vec3 attenuation = vec3(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        tCurrent += segmentLength;

    }
    //return vec3(1.0, 0.0, 0.0);
    //return vec3(sunDirection);
    return (sumR * betaR * phaseR + sumM * betaM * phaseM) * 20.0;
}

void main()
{   
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * (vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0) * far_plane);
    vec3 dir  = normalize(rd.xyz);
    orig = orig + vec3(0.0, earthRadius, 0.0);

    // Read Pre Data
    vec3 M_pos = orig + dir * 3.0;
    vec4 M_ndc = projection_pre * view_pre * vec4(M_pos, 1.0);
    M_ndc = M_ndc / M_ndc.w;
    vec2 M_uv = M_ndc.xy * 0.5 + 0.5;
    vec3 M_dir = normalize(M_pos - LastViewPosWS);

    // Read history
    vec4 pos_tex;
    vec4 scat_tex;
    vec4 flags = vec4(1e-5);

    // 如果是第一帧，就不读去历史数据区
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

    //历史拒绝算法
    // ToDo

    // 范围判断
    if(inRange(M_uv) < -0.05)
    {
        pos_tex = vec4(orig, 0.0);
        scat_tex = vec4(0.0, 0.0, 0.0, 1.0);
    }


    // 计算实际的大气颜色
    vec3 sun = normalize(-dirLights[0].direction);
    mat3 rot = rotate_around_x(-abs(sin(iTime / 12.0)) * 70.0);
	sun *= rot;
    vec3 color = computeIncidentLight_T(orig, dir, sun, 0.0, 100000.0);

    // 添加云雾
    color += Gain_33(cloud_Patu(dir), 1.0) * 1.2;

    color = GammaCorrection(color);

    FragColor = vec4(color, 1.0);
    //FragColor = vec4(TexCoords,1.0);
}