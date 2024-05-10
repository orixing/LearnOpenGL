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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
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

        float shadow = ShadowCalculation(FragPosLightSpace);       
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));    

    //FragColor = vec4(1.0);
    //FragColor = vec4(shadow,shadow,shadow,1.0);
    //FragColor = vec4(-FragPosLightSpace.x,-FragPosLightSpace.y,FragPosLightSpace.z,1.0);
    FragColor = vec4(lighting,1.0);
};