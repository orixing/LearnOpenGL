#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos[2];

out VS_OUT {
vec2 texCoords;
vec3 position;
vec3 Normal;
vec3 fragPos;
vec3 LightPos[2];
mat4 model;
} vs_out;


void main()
{
	vs_out.texCoords = vec2(aTexCoords.x, 1-aTexCoords.y);
	vs_out.position = aPos;
   gl_Position = projection * view * instanceMatrix  * vec4(aPos, 1.0);
   vs_out.Normal =  mat3(transpose(inverse(view * instanceMatrix ))) * vec3(aNormal.x,aNormal.y,aNormal.z);
   vs_out.fragPos = vec3(view * instanceMatrix  * vec4(aPos, 1.0));
   vs_out.LightPos[0] = vec3(view * vec4(lightPos[0], 1.0));
   vs_out.LightPos[1] = vec3(view * vec4(lightPos[1], 1.0));
   vs_out.model = instanceMatrix;
}