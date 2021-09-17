#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D noisyTexture;

void main()
{   
    vec3 orig = vec3(0.0, earthRadius + 1.0, 0.0);
    vec3 dir = normalize(TexCoords);
    vec3 sun = normalize(-dirLights[0].direction);
    vec3 color = computeIncidentLight(orig, dir, sun, 0.0, 100000.0);

    color += Gain_33(cloud_Patu(dir), 1.0) * 1.2;

    color = GammaCorrection(color);

    FragColor = vec4(color, 1.0);
    //FragColor = vec4(TexCoords,1.0);
}