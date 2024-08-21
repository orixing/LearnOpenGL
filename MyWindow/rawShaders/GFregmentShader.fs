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


const float NEAR = 0.1; // ͶӰ����Ľ�ƽ��
const float FAR = 100.0f; // ͶӰ�����Զƽ��
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // �ص�NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}


void main()
{    
    // �洢��һ��G���������е�Ƭ��λ������
    gNormal = vec4(normalize(Normal), 1.0);
    // ͬ���洢��ÿ����Ƭ�η��ߵ�G������
    gPosition.xyz = FragPos;
    gPosition.a=1.0;
    // ���������ÿ����Ƭ����ɫ
    gAlbedo = vec4(texture(albedoTex, TexCoords).rgb,1.0);
    // �洢����ǿ�ȵ�gAlbedoSpec��alpha����
    gExtra = vec4(materialType, metallic, roughness, 1.0);
}  