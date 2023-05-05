#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = FoveatedRendering_Pass_2(screenTexture, TexCoords);
    FragColor = vec4(col.rgb, 1.0);
} 