#version 330 core
in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D ssaoInput;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x <= 2; ++x) 
    {
        for (int y = -2; y <= 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }
    fragColor = vec4(result / (5.0 * 5.0), result / (5.0 * 5.0), result / (5.0 * 5.0) ,1.0);
}