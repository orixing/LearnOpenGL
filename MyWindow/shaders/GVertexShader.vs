#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;


out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main()
{
	TexCoords = vec2(aTexCoords.x, 1-aTexCoords.y);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	Normal =  transpose(inverse(mat3(view * model))) * vec3(aNormal.x,aNormal.y,aNormal.z);
	FragPos = vec3(view * model * vec4(aPos, 1.0));
	FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
}