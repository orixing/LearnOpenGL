#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 texCoord;
//uniform float horizentalAbs;
//uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
 
   //vec4 newPos = transform *  vec4(aPos , 1.0);
   //newPos.x = max(-1.0, newPos.x + horizentalAbs);
   //newPos.x = min(1.0,newPos.x);
   gl_Position =projection * view * model * vec4(aPos, 1.0);
   ourColor = aColor;
   texCoord = aTexCoord;
}