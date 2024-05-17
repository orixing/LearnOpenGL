#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform mat4 lightSpaceMatrix;


out vec2 texCoords;
out vec3 Normal;
out vec3 fragPos;
out vec3 LightPos;
out vec4 FragPosLightSpace;



void main()
{
	texCoords = vec2(aTexCoords.x, 1-aTexCoords.y);

   gl_Position = projection * view * model * vec4(aPos, 1.0);
   Normal =  mat3(transpose(inverse(view * model))) * vec3(aNormal.x,aNormal.y,aNormal.z);
   fragPos = vec3(view * model * vec4(aPos, 1.0));
   LightPos= vec3(view * vec4(lightPos, 1.0));
   FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
}