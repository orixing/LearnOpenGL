#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec3 posAsColor;
out vec2 texCoord;
uniform float horizentalAbs;

void main()
{
   float x = max(-1.0, aPos.x + horizentalAbs);
   x = min(1.0,x);
   gl_Position = vec4(x, -aPos.y, aPos.z, 1.0);
   ourColor = aColor;
   posAsColor = vec3(x, -aPos.y, aPos.z);
   texCoord = aTexCoord;
}