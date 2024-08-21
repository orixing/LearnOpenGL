#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gExtra;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
uniform float materialType;
uniform float metallic;
uniform float roughness;
uniform sampler2D albedoTex;


const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 100.0f; // 投影矩阵的远平面
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}


void main()
{    
    // 存储第一个G缓冲纹理中的片段位置向量
    gNormal = vec4(normalize(Normal), 1.0);
    // 同样存储对每个逐片段法线到G缓冲中
    gPosition.xyz = FragPos;
    gPosition.a=1.0;
    // 和漫反射对每个逐片段颜色
    gAlbedo = vec4(texture(albedoTex, TexCoords).rgb,1.0);
    // 存储镜面强度到gAlbedoSpec的alpha分量
    gExtra = vec4(materialType, metallic, roughness, 1.0);
}  