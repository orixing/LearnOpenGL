#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform mat4 lightSpaceMatrix;
uniform sampler2D normalTex;

out VS_OUT {
 vec2 TexCoords;
 vec3 fragPos;
 vec3 LightPos;
 vec3 Normal;
 vec4 FragPosLightSpace;
  vec3 position;
} vs_out;


void main()
{
	vs_out.TexCoords = vec2(aTexCoords.x, 1-aTexCoords.y);
    vs_out.position =aPos;
	//vs_out.Normal = normalize(pow(texture(normalTex, vec2(aTexCoords.x, 1-aTexCoords.y)).rgb, vec3(2.2)));
    vs_out.Normal =  mat3(transpose(inverse(view * model))) * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.fragPos = vec3(view * model * vec4(aPos, 1.0));
    vs_out.LightPos= vec3(view * vec4(lightPos, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);
}