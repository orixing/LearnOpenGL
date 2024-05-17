#version 330 core
//layout (location = 0) out vec3 gPosition;
//layout (location = 1) out vec3 gNormal;
//layout (location = 2) out vec4 gAlbedoSpec;
//layout (location = 3) out vec4 gFragPosInLight;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gAlbedoSpec;
layout (location = 4) out vec4 gFragPosInLight;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D spotTex;


void main()
{    
    FragColor = vec4(1-FragPosLightSpace.zzz/FragPosLightSpace.w, 1.0);
    // 存储第一个G缓冲纹理中的片段位置向量
    gNormal = vec4(normalize(Normal), 1.0);
    // 同样存储对每个逐片段法线到G缓冲中
    //gNormal = normalize(Normal);
    gPosition = vec4(FragPos, 1.0);
    // 和漫反射对每个逐片段颜色
    gAlbedoSpec = vec4(texture(spotTex, TexCoords).rgb,1.0);
    // 存储镜面强度到gAlbedoSpec的alpha分量
    //gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
    gFragPosInLight = vec4(FragPosLightSpace.xyz/FragPosLightSpace.w,1.0);
}  