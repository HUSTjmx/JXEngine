#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;


void main()
{   
    vec3 d_color = texture(diffuseTexture, fs_in.TexCoords).rgb * baseColor;

    vec3 norm = normalize(fs_in.Normal);

    vec3 viewDir = normalize(viewPosWS - fs_in.FragPos);

	vec3 diffuse_color = BaseColorReMap(d_color, metallic);

	float r = RoughnessReMap(roughness);

	vec3 f0 = GetF0_All(baseColor, metallic, reflectance);

    vec3 result = vec3(0.0) + baseColor * vec3(0.01);

    float NoL;

	for(int i = 0;i < dirLightsNum; i++)
	{
		//light_Intensity * light_color * BRDF * NoL 
		NoL = clamp(dot(norm, normalize(-dirLights[i].direction)), 0.0, 1.0);
		result += ShadowCalculation_PCSS_v1(fs_in.FragPosLightSpace, shadowMap, NoL) * GetDirLightColor(i) * GetDirLightIllumiance(i) 
		* Standard_BRDF_Torrance_DVF(diffuse_color, viewDir, -dirLights[i].direction, norm, r, f0, 1.0) * NoL;
	}

    for(int j = 0; j < pointLightsNum; ++j)
    {
        NoL = clamp(dot(norm, normalize(pointLights[j].position - fs_in.FragPos)), 0.0, 1.0);
        result += GetPointLightIllumiance(j, fs_in.FragPos) * GetPointLightColor(j) 
        * Standard_BRDF_Torrance_DVF(diffuse_color, viewDir, normalize(pointLights[j].position - fs_in.FragPos), norm, r, f0, 1.0) * NoL;
    }

    result = GammaCorrection(result);
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(vec3(ShadowCalculation(fs_in.FragPosLightSpace, NoL)), 1.0);
}