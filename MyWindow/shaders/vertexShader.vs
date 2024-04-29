#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos[2];

out vec2 TexCoords;
out vec3 Normal;
out vec3 fragPos;
out vec3 LightPos[2];
void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   TexCoords = vec2(aTexCoords.x, 1-aTexCoords.y);
   Normal = vec3(-aNormal.x,aNormal.y,-aNormal.z);
   fragPos = vec3(view * model * vec4(aPos, 1.0));
   LightPos[0] = vec3(view * vec4(lightPos[0], 1.0));
   LightPos[1] = vec3(view * vec4(lightPos[1], 1.0));
}