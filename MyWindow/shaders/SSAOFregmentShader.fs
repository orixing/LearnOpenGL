#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

// 屏幕的平铺噪声纹理会根据屏幕分辨率除以噪声大小的值来决定
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); // 屏幕 = 800x600

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
        // 获取样本位置
        vec3 sample = TBN * samples[i]; // 切线->观察空间
        float radius = 1.5;
        sample = fragPos + sample * radius; 

        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // 观察->裁剪空间
        offset.xyz /= offset.w; // 透视划分
        offset.xyz = offset.xyz * 0.5 + 0.5; // 变换到0.0 - 1.0的值域

        float sampleDepth = texture(gPositionDepth, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + 0.0001 ? 1.0 : 0.0) * rangeCheck;           
    }


    occlusion = 1.0 - (occlusion / kernelSize);

    //vec4 test = projection * texture(gPositionDepth, TexCoords);
    //float d= test.z/test.w;

    //vec4 test=projection * vec4(fragPos,1.0); // 观察->裁剪空间
       // test.xyz /= test.w; // 透视划分
       // test.xyz = test.xyz * 0.5 + 0.5; // 变换到0.0 - 1.0的值域
        //float d = -texture(gPositionDepth, test.xy).z;
    FragColor = vec4(occlusion,occlusion,occlusion,1.0);  
}