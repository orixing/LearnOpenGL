#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoords;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightPos;
out vec3 LightPos;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   TexCoords = aTexCoords;
   Normal = mat3(transpose(inverse(view * model))) *  vec3(0.0, 1.0, 0.0);
   LightPos = vec3(view * vec4(lightPos, 1.0));
   FragPos = vec3(view * model * vec4(aPos, 1.0));
   FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
}