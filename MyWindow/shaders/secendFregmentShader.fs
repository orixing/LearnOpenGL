#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec3 posAsColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
   // FragColor = vec4(0.0, 0.5, 0.0, 1.0);
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
};