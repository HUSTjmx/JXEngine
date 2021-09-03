#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{             
    vec4 texColor = texture(texture1, TexCoords);
    
    AlphaTest(texColor.a, 0.1f);

    FragColor = texColor;
    //FragColor = vec4(TexCoords.x, TexCoords.y, 0, 1.0);
}