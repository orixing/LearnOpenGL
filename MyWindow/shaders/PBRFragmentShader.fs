#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 LightPos;
uniform vec3 lightColor;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

uniform sampler2D shadowMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texSSAO;
uniform sampler2D gAlbedo;
uniform sampler2D gFragPosLightSpace;
uniform sampler2D gExtra;


const float PI = 3.14159265359;

float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main()
{
      vec4 FragPosLightSpace =  texture(gFragPosLightSpace, TexCoords);
	  vec3 WorldPos = texture(gPosition, TexCoords).rgb;
      vec3 Normal = texture(gNormal, TexCoords).rgb;
      vec3 albedo = texture(gAlbedo, TexCoords).rgb;
      float ao = texture(texSSAO, TexCoords).r;
      bool isPBR = texture(gExtra, TexCoords).r == 1.0;
      float metallic = texture(gExtra, TexCoords).g;
      float roughness = texture(gExtra, TexCoords).b;

    vec3 N = normalize(Normal); 
    vec3 V = normalize(-WorldPos);//世界坐标到视线的向量
    vec3 Lo = vec3(0.0);
    vec3 L = normalize(LightPos - WorldPos);//世界坐标到光的向量
    vec3 H = normalize(V + L);
    float distance    = length(LightPos - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = lightColor * attenuation; 

    albedo = pow(albedo, vec3(2.2));

    if(isPBR){
        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        vec3 F  = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        float NDF = DistributionGGX(N, H, roughness);       
        float G   = GeometrySmith(N, V, L, roughness); 
        vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
        vec3 specular = nominator / denominator;  

        float shadow = ShadowCalculation(FragPosLightSpace, N, L);   

        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1-shadow);

        //漫反射环境光 1-F * 1-m * c * L（PI已经处理过了）* ao

        vec3 ambientDiffuse = (1 - F) * (1-metallic) * texture(irradianceMap, N).rgb * ao * albedo;

        //镜面环境光
        vec3 R = reflect(-V, N);   
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 envLight = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;  
        vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 envSpecular = envLight * (F0 * envBRDF.x + envBRDF.y) * ao;


        //vec3 ambient = vec3(0.0005) * albedo * ao;
        vec3 color   = 0.18*ambientDiffuse  +  0.18*envSpecular + Lo;  
        //vec3 color   =  vec3(F0 * envBRDF.x + envBRDF.y);

        color = color / (color + vec3(1.0));
        color = pow(color, vec3(1.0/2.2)); 
        FragColor = vec4(color, 1.0);
    }
    
     else{
      vec3 ambLightIntensity = vec3(1.0,1.0,1.0);
      vec3 ka = vec3(0.0005,0.0005,0.0005);
      vec3 ambient = ka * ambLightIntensity * ao;

      vec3 lightIntensity = vec3(5.0,5.0,5.0);
      vec3 kd = albedo;
      float distance = length(LightPos-WorldPos);
      vec3 diffuse = kd * max(0.0, dot(N, L)) * lightIntensity / distance;

      vec3 ks = vec3(0.7937, 0.7937, 0.7937);
      vec3 halfV = normalize(V + L);
      float spec = pow(max(dot(halfV, N), 0.0), 150);
      vec3 specular = ks * spec * lightIntensity / distance;

      float shadow = ShadowCalculation(FragPosLightSpace, N, L);
      vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));  
      
      lighting = pow(lighting, vec3(1.0/2.2)); 
      FragColor = vec4(lighting,1.0);
    }

}