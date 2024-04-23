#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec3 posAsColor;
void main()
{
	FragColor = vec4(posAsColor, 1.0);
}