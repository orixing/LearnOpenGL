#version 330 core
//layout (location = 0) out vec3 gPosition;
//layout (location = 1) out vec3 gNormal;
//layout (location = 2) out vec4 gAlbedoSpec;
//layout (location = 3) out vec4 gFragPosInLight;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 gPositionDepth;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gAlbedoSpec;
layout (location = 4) out vec4 gFragPosInLight;
layout (location = 6) out vec4 gExtra;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
uniform bool isPBR;
uniform sampler2D spotTex;


const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 100.0f; // 投影矩阵的远平面
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}


void main()
{    
    FragColor = vec4(1-FragPosLightSpace.zzz/FragPosLightSpace.w, 1.0);
    // 存储第一个G缓冲纹理中的片段位置向量
    gNormal = vec4(normalize(Normal), 1.0);
    // 同样存储对每个逐片段法线到G缓冲中
    //gNormal = normalize(Normal);
    gPositionDepth.xyz = FragPos;
    gPositionDepth.a=1.0;
    // 和漫反射对每个逐片段颜色
    gAlbedoSpec = vec4(texture(spotTex, TexCoords).rgb,1.0);
    // 存储镜面强度到gAlbedoSpec的alpha分量
    //gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
    gFragPosInLight = vec4(FragPosLightSpace.xyz/FragPosLightSpace.w,1.0);
    gExtra = isPBR?vec4(1.0):vec4(0.0);
}  