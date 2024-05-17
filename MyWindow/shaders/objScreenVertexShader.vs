#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
uniform vec3 lightPos;
uniform mat4 view;


out vec2 TexCoords;
out vec3 LightPos;
out  vec2 testCoords;


void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    testCoords = aPos.xy;
    TexCoords = aTexCoords;
    LightPos= vec3(view * vec4(lightPos, 1.0));
}