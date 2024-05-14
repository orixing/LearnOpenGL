#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 fragPos;
in vec4 FragPosLightSpace;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D spotTex;
uniform sampler2D shadowMap;
in vec3 LightPos;

float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // ±ä»»µ½[0,1]µÄ·¶Î§
    projCoords = projCoords * 0.5 + 0.5;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float currentDepth = projCoords.z;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
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
    return shadow / 9;
}


void main()
{
        vec3 ambLightIntensity = vec3(1.0,1.0,1.0);
        vec3 ka = vec3(0.05,0.05,0.05);
        vec3 ambient = ka * ambLightIntensity;

        vec3 lightIntensity = vec3(5.0,5.0,5.0);
        vec3 kd = vec3(texture(spotTex, TexCoords));
        float distance = length(LightPos-fragPos);

        vec3 lightDir = normalize(LightPos - fragPos);
        vec3 n = normalize(Normal);
        vec3 diffuse = kd * max(0.0, dot(n, lightDir)) * lightIntensity / distance;

        vec3 ks = vec3(0.7937, 0.7937, 0.7937);
        vec3 viewPos = vec3(0.0,0.0,0.0);
        vec3 freg2View = normalize(viewPos - fragPos);
        vec3 halfV = normalize(freg2View + lightDir);
        float spec = pow(max(dot(halfV, n), 0.0), 150);
        vec3 specular = ks * spec * lightIntensity / distance;

        float shadow = ShadowCalculation(FragPosLightSpace, n, lightDir);       
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));    

    //FragColor = vec4(1.0);
    //FragColor = vec4(shadow,shadow,shadow,1.0);
    //FragColor = vec4(-FragPosLightSpace.x,-FragPosLightSpace.y,FragPosLightSpace.z,1.0);
    FragColor = vec4(lighting,1.0);
};