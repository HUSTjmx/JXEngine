#version 330 core
#define MAP_VERSION 1.0
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D preTexture;
uniform mat4 view_inv;
uniform mat4 projection_inv;
uniform int blurDir;

float HSV_PDF(float e)
{
    const float M = 1.0;
    if(e <= 5.79)return 1.0 * M * 1.0;
    return 7.49 / pow(0.3 * e + 1.0, 2.0) * M;
}

float Map_HSV(float v)
{
    return exp(1.0 - v) - 1.0;
}


void main()
{
    vec4 rd = view_inv * projection_inv * vec4(ReMapNDC_V3(vec3(TexCoords, 1.0)), 1.0);
    vec3 dir = normalize(rd.xyz);
    vec3 centerDir = normalize(view_center_dir);
    float e = degrees(acos(dot(dir, centerDir)));
    float v = HSV_PDF(e);
    float len = Map_HSV(v)  * 3;
    //FragColor = vec4(vec3(len), 1.0);
    if(blurDir == 0)
        FragColor = vec4(GaussianBlur_X(screenTexture, TexCoords, len), 1.0);
    else
        FragColor = vec4(GaussianBlur_Y(screenTexture, TexCoords, len), 1.0);
 //FragColor = vec4(texture(preTexture, TexCoords).xyz, 1.0);
    //FragColor = vec4(vec3(HSV_PDF(e)), 1.0);
   // FragColor = vec4(dir, 1.0);

}