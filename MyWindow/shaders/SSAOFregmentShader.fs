#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

// ��Ļ��ƽ����������������Ļ�ֱ��ʳ���������С��ֵ������
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); // ��Ļ = 800x600

void main()
{
    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    int kernelSize = 64;
    for(int i = 0; i < kernelSize; ++i)
    {
        // ��ȡ����λ��
        vec3 sample = TBN * samples[i]; // ����->�۲�ռ�
        float radius = 1.5;
        sample = fragPos + sample * radius; 

        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // �۲�->�ü��ռ�
        offset.xyz /= offset.w; // ͸�ӻ���
        offset.xyz = offset.xyz * 0.5 + 0.5; // �任��0.0 - 1.0��ֵ��

        float sampleDepth = texture(gPositionDepth, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + 0.0001 ? 1.0 : 0.0) * rangeCheck;           
    }


    occlusion = 1.0 - (occlusion / kernelSize);

    //vec4 test = projection * texture(gPositionDepth, TexCoords);
    //float d= test.z/test.w;

    //vec4 test=projection * vec4(fragPos,1.0); // �۲�->�ü��ռ�
       // test.xyz /= test.w; // ͸�ӻ���
       // test.xyz = test.xyz * 0.5 + 0.5; // �任��0.0 - 1.0��ֵ��
        //float d = -texture(gPositionDepth, test.xy).z;
    FragColor = vec4(occlusion,occlusion,occlusion,1.0);  
}