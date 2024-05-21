#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 LightPos;
uniform sampler2D shadowMap;
uniform sampler2D gFragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gFragPosLightSpace;
uniform sampler2D texSSAO;
in  vec2 testCoords;

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
                float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0001);
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
      vec4 FragPosLightSpace =  texture(gFragPosLightSpace, TexCoords);
	  vec3 fragPos = texture(gPosition, TexCoords).rgb;
      vec3 Normal = texture(gNormal, TexCoords).rgb;
      vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
     float ssao = texture(texSSAO, TexCoords).r;

      vec3 ambLightIntensity = vec3(1.0,1.0,1.0);
      vec3 ka = vec3(0.05,0.05,0.05);
      vec3 ambient = ka * ambLightIntensity * ssao;

      vec3 lightIntensity = vec3(5.0,5.0,5.0);
      vec3 kd = Albedo;
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



     // float shadow = 0.0;
      vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));    

    //FragColor = vec4(texture(gNormal, TexCoords).rgb, 1.0);
    //FragColor = vec4(shadow,shadow,shadow,1.0);
    //FragColor = vec4(-FragPosLightSpace.x,-FragPosLightSpace.y,FragPosLightSpace.z,1.0);
    //FragColor = vec4(1.0,1.0,1.0,1.0);
    if(Normal == vec3(0.0,0.0,0.0)){
        FragColor = vec4(0.0,0.0,0.0,1.0);
    } else{
        FragColor = vec4(lighting,1.0);
        //FragColor = vec4(texture(shadowMap, projCoords.xy).rgb,1.0);
        //FragColor = vec4(1.0-shadow,1.0-shadow,1.0-shadow,1.0);
        //FragColor = vec4(texture(gNormal, vec2(testCoords.x/2+0.5,testCoords.y/2+0.5 )).rgb, 1.0);
    }
    //FragColor = vec4(texture(shadowMap, testCoords.xy).r,0.0,0.0,1.0);
    //float depthValue = texture(shadowMap, testCoords.xy).r;
    //FragColor = vec4(vec3(depthValue),1.0);
    //FragColor = vec4(shadow,shadow,shadow,1.0);
};