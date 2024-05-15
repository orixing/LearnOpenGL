#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D groundTex;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec3 Normal;
in vec3 LightPos;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // ±ä»»µ½[0,1]µÄ·¶Î§
    projCoords = projCoords * 0.5 + 0.5;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float currentDepth = projCoords.z;
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            projCoords  = vec3(projCoords.xy + vec2(x, y) * texelSize,projCoords.z);
            if(projCoords.x <0.0 || projCoords.x >1.0 || projCoords.y <0.0 || projCoords.y >1.0){
                shadow += 0.0;
            } else{
                float pcfDepth = texture(shadowMap, projCoords.xy).r; 
                float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;      
            }
        }    
    }
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow / 25;
}

void main()
{
    vec4 color = texture(groundTex, TexCoords);
    float shadow = ShadowCalculation(FragPosLightSpace,normalize(Normal), normalize(LightPos-FragPos));
    color = color * (1-shadow);

    FragColor = vec4(color.xyz,1.0);
    //FragColor = vec4(normalize(Normal),1.0);
    //FragColor = vec4(shadow,shadow,shadow,1.0);
}