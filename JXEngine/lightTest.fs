#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;


void main()
{
	vec3 norm = normalize(Normal);

    vec3 viewDir = normalize(viewPosWS - FragPos);

	vec3 result = vec3(0.0);

	vec3 diffuse_color = BaseColorReMap(baseColor, metallic);

	float r = RoughnessReMap(roughness);

	vec3 f0 = GetF0_All(baseColor, metallic, reflectance);

	for(int i = 0;i < dirLightsNum; i++)
	{
		//light_Intensity * light_color * BRDF * NoL 
		float NoL = clamp(dot(norm, normalize(-dirLights[i].direction)), 0.0, 1.0);
		result += GetDirLightColor(i) * GetDirLightIllumiance(i) 
		* Standard_BRDF_Torrance_DVF(diffuse_color, viewDir, -dirLights[i].direction, norm, r, f0, 1.0) * NoL;
	}

	for(int i = 0; i < pointLightsNum; i++)
	{
		//light_Intensity * light_color * BRDF * NoL 
		vec3 L = normalize(pointLights[i].position - FragPos);
		float NoL = clamp(dot(norm, L), 0.0, 1.0);
		result = result + GetPointLightColor(i) * GetPointLightIllumiance(i, FragPos)
		* Standard_BRDF_Torrance_DVF(diffuse_color, viewDir, L, norm, r, f0, 1.0) * NoL; 
	}	

	FragColor =  vec4(result, 1.0);
}
