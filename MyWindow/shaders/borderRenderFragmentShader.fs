#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D colorTex;

void main()
{
     FragColor = vec4(texture(colorTex, TexCoords).rgb,1.0);
     //FragColor = vec4(TexCoords,0.0,1.0);
};