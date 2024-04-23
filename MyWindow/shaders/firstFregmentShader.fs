#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec3 posAsColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	//FragColor = vec4(posAsColor, 1.0);
	vec2 mirrorTexCoord = vec2(1.0-texCoord.x, texCoord.y);
	FragColor = mix(texture(texture1, texCoord), texture(texture2, mirrorTexCoord), 0.5);
}