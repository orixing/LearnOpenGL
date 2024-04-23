#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec3 posAsColor;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main()
{
   // FragColor = vec4(0.0, 0.5, 0.0, 1.0);
    FragColor = texture(ourTexture, texCoord) * vec4(0.0, 0.5, 0.0, 1.0);
};