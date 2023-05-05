#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D FrontDepthTex;
uniform sampler2D GrayNoiseMap_S;
uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 projection;
uniform mat4 projection_inv;

vec3 obj_pos = vec3(3.0);
float r1 = 1.0;

// Tool function for Media integration Cloud.
// ...
void getParticipatingMedia(sampler2D noiseTexture ,out float sigmaS, out float sigmaE, in vec3 pos, vec3 objPos)
{
    float Fog = 0.5 + D_FOG_NOISE * 1.0 * clamp(displacementSimple_FBM_v3(noiseTexture, pos.xy * 0.2  + iTime * 0.3), 0.0, 1.0);
    Fog = 0.3 * clamp((Fog - length(objPos - pos)) / r1, 0.0, 1.0);

    float fogFactor = 2.0 + D_STRONG_FOG * 6.0;

    sigmaS = Fog * fogFactor;
    float sigmaA = 0.0;
    sigmaE = max(0.000001, sigmaS + sigmaA);
} 

float volumetricShadow2(sampler2D noiseTexture, in vec3 from, in vec3 to, vec3 obj_pos)
{
    float numStep = 5.0;
    float shadow = 1.0;
    float sigmaE , sigmaS;

    float dd = length(to - from) / numStep;
    for(float s = 0.5; s < (numStep - 0.1); s += 1.0)
    {
        vec3 pos = from + (to - from) * (s / (numStep));

        getParticipatingMedia(noiseTexture, sigmaS, sigmaE, pos, obj_pos);

        shadow *= exp(-sigmaE * dd);
    }
    //return 1.0;
    return shadow;
}

vec3 TraceFloor(vec3 orig, vec3 rd, float depth)
{
    float sigmaS = 0.0;
    float sigmaA = 0.0;
    float sigmaE = sigmaS + sigmaA;

    float transmittance = 1.0;

    vec3 scatteredLight = vec3(0.0, 0.0, 0.0);
    
    // the distance that has moved.
    // ...
    float d = 0.0;

    // Current march pos.
    // ...
    vec3 p = vec3(0.0 , 0.0, 0.0);

    // Step length.
    // ...
    float dd = 0.0;

    // Iter times.
    // ...
    int iternum = 100;
    float dis = 0.0;

    vec2 ce = sphIntersect( orig, rd, obj_pos, r1 );

    if(ce.x == -1.0 && ce.y == -1.0) return texture(screenTexture, TexCoords).xyz;

    dd = abs(ce.x - ce.y) / iternum;
    d = min(ce.x, ce.y);

    for(int i = 0; i < iternum; ++i)
    {
        p = orig + rd * d;

        getParticipatingMedia(GrayNoiseMap_S, sigmaS, sigmaE, p, obj_pos);

        for(int j = 0; j < dirLightsNum; ++j)
        {
            float mu = clamp(dot(rd, normalize(dirLights[j].direction)), 0.0, 1.0);
            vec3 S = GetDirLightIllumiance(j) * GetDirLightColor(j) * sigmaS * PhaseFunc_RAYLEIGH(mu)
             * volumetricShadow2(GrayNoiseMap_S, p, p + dirLights[j].direction * 100.0, obj_pos);
            vec3 Sint = (S - S * exp(-sigmaE * dd)) / sigmaE;
            scatteredLight += transmittance * Sint * 0.6;
        }

        for(int j = 0; j < pointLightsNum; ++j)
        {
            vec3 lightDir = normalize(pointLights[j].position - p);
            float mu = clamp(dot(rd, -lightDir), 0.0, 1.0);
            vec3 S = GetPointLightColor(j) * GetPointLightIllumiance(j, p) * sigmaS * PhaseFunc_RAYLEIGH(mu)
            * volumetricShadow2(GrayNoiseMap_S, p, pointLights[j].position, obj_pos);
            vec3 Sint = (S - S * exp(-sigmaE * dd));
            scatteredLight += transmittance * Sint * 0.6;
        }
        
        // ----------------------------------
        vec4 temp = projection * view * vec4(p, 1.0);
        vec3 pos = temp.xyz / temp.w;
        pos = (pos + 1.0) / 2.0;
        float z = LinearizeDepth(pos.z, near_plane, far_plane) / far_plane;
        if(depth < z + 0.001) return texture(screenTexture, TexCoords).xyz;

        // --------------------------------
        d += dd;
        if(d > 100.0)break;
    }
    //return vec3(dis);
    //if(length(scatteredLight) < 0.03) return texture(screenTexture, TexCoords).xyz;
    return mix(GammaCorrection(scatteredLight), texture(screenTexture, TexCoords).xyz, 1.0 - length(scatteredLight));
}

void main()
{
    float depth = texture(FrontDepthTex, TexCoords).r;
    depth = LinearizeDepth(depth, near_plane, far_plane) / far_plane;
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * vec4(vec3(ReMapNDC_V2(TexCoords), 0.99), 1.0);
    vec3 dir = normalize(rd.xyz / rd.w);

    vec4 temp = projection * view * vec4(rd/rd.w);
    vec3 pos = temp.xyz / temp.w;
    pos = (pos + 1.0)/2.0;
    //depth = texture(FrontDepthTex, pos.xy).r;
    //depth = LinearizeDepth(depth, near_plane, far_plane) / far_plane;


    vec3 col = TraceFloor(orig, dir, depth);
   // col = GammaCorrection(col);
    //col = vec3(depth);
    //col = normalize(rd);
    //col = uv.xxy;
    //col = TexCoords.xxy;
   // col = normalize(rd);
    //col = normalize(rd);
    //col =vec3(depth);
    FragColor = vec4(col, 1.0);
    //FragColor = texture(screenTexture, TexCoords);
} 