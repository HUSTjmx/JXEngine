#version 330 core
#define MAP_VERSION 1.0
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;
in vec2 TexCoords;

uniform sampler2D noisyTexture;
uniform sampler2D screenTexture;

void main()
{
    vec3 col = MysteryMoutains_v1(noisyTexture, TexCoords);
    
    //FragColor = vec4(TexCoords.xyy, 1.0);
    FragColor = vec4(col, 1.0);

}