#version 330 core
out vec4 FragColor;

uniform vec3 objColor;
uniform vec3 lightColor;

void main()
{
   // FragColor = vec4(0.0, 0.5, 0.0, 1.0);
    FragColor = vec4(objColor * lightColor, 1.0);
};