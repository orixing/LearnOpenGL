#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragWorldPos;

void main()
{
   gl_Position =projection * view * model * vec4(aPos, 1.0);
   fragWorldPos = vec3(model * vec4(aPos,1.0));
   //normal = aNormal;
   normal = mat3(transpose(inverse(model))) * aNormal;
}