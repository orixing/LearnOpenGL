#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D grassTex;

void main()
{
    vec4 color = texture(grassTex, TexCoords);
    FragColor = color;
}