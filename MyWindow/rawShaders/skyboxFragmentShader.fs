#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    vec3 envColor = texture(skybox, TexCoords).rgb;
    FragColor = vec4(envColor, 1.0);
}