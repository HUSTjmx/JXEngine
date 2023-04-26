#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D noisyTexture;
uniform sampler2D screenTexture;

uniform mat4 view_inv;
uniform mat4 projection_inv;

void main()
{
    vec3 orig = viewPosWS;
    vec4 rd = view_inv * projection_inv * vec4(vec3(ReMapNDC_V2(TexCoords), 0.99), 1.0);
    vec3 dir = normalize(rd.xyz / rd.w);

}