#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    vec3 envColor = texture(skybox, TexCoords).rgb;
    //vec3 envColor = textureLod(skybox, TexCoords,4.2).rgb;
    FragColor = vec4(envColor, 1.0);
}