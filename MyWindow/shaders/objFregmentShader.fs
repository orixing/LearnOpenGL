#version 330 core
out vec4 FragColor;
in vec3 fragColor;

uniform vec3 objColor;

void main()
{

    FragColor = vec4(fragColor * objColor, 1.0);
};