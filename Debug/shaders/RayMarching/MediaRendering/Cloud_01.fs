#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosViewSpace;
    vec2 TexCoords;
} fs_in;


uniform sampler2D BackDepthMap;
uniform sampler2D GrayNoiseMap_S;

void main()
{   
    vec3 norm = normalize(fs_in.Normal);

    vec3 viewDir = normalize(viewPosWS - fs_in.FragPos);

    vec3 result = vec3(0.0) + baseColor * vec3(0.01);

    float NoL;

    vec3 uv = fs_in.FragPosViewSpace.xyz / fs_in.FragPosViewSpace.w;
    uv = uv * 0.5 + 0.5;

    float depth_c = LinearizeDepth(uv.z, near_plane, far_plane) / far_plane;

    float depth = texture(BackDepthMap, uv.xy).r;
    depth = LinearizeDepth(depth, near_plane, far_plane) / far_plane;

    float lenght = depth - depth_c;

    vec4 scatTrans = cloud_SebH(GrayNoiseMap_S, fs_in.FragPos, viewDir, lenght);

    vec3 color = scatTrans.xyz;

    //result = vec3(lenght) ;
    result = color;
    result = GammaCorrection(result);

    //result = texture(GrayNoiseMap_S, fs_in.TexCoords).rrr;
    //result = vec3(sin(iTime));
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(vec3(ShadowCalculation(fs_in.FragPosLightSpace, NoL)), 1.0);
}