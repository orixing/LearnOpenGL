#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
    //FragColor = vec4(1.0,1.0,1.0,1.0);
}


//#version 330 core
//out vec4 color;
//in vec2 TexCoords;

//uniform sampler2D screenTexture;

//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0 - 1.0; // Back to NDC 
//    return (2.0 * 1.0 * 100.0) / (100.0 + 1.0 - z * (100.0 - 1.0));
//}

//void main()
//{             
    //float depthValue = texture(screenTexture, TexCoords).r;
    //color = vec4(vec3(LinearizeDepth(depthValue) / 100.0), 1.0); // perspective
    // color = vec4(vec3(depthValue), 1.0); // orthographic
//}