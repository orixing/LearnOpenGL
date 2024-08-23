#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform mat4 view;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texSSAO;
uniform sampler2D gAlbedo;
uniform sampler2D gExtra;


const float PI = 3.141592653589793;
const float PI2 = 6.283185307179586;
const int NUM_SAMPLES = 200;

vec2 poissonDisk[NUM_SAMPLES];

struct Light {
    vec3 position;
    vec3 intensity;
    mat4 viewMat;
    mat4 projMat;
    sampler2D shadowMap;
};

uniform int lightNum;
uniform Light lights[10];

float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const  float a = 12.9898, b = 78.233, c = 43758.5453;
	float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

void poissonDiskSamples( const in vec2 randomSeed ) {

  float ANGLE_STEP = PI2 * float( 10 ) / float( NUM_SAMPLES );
  float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

  float angle = rand_2to1( randomSeed ) * PI2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}


//uv:着色点在shadowmap上的uv
float PCSSGetAvgDepth(int lightIndex, vec4 fragPosLightSpace, vec2 uv,float fragZ){
    poissonDiskSamples(uv);
    vec2 texelSize = 1.0 / textureSize(lights[lightIndex].shadowMap, 0);

    vec4 fragPosLightView = inverse(lights[lightIndex].projMat) * fragPosLightSpace;
    float radiusF = fragPosLightView.z*0.8/(0.1+fragPosLightView.z)/texelSize.x;

    int radius = int(radiusF)+1;
    float count =0.0;
    float totalDepth =0.0;
    bool noBlocker = true;
    for(int i = 0;i<NUM_SAMPLES;i++){
        vec2 sampleCoord = uv + texelSize * vec2(radius) * poissonDisk[i];
        float blockerDepth = texture(lights[lightIndex].shadowMap, sampleCoord).r;
        //如果遮挡住了，则有效，算作平均深度
        if(fragZ - 0.001 > blockerDepth){
            noBlocker = false;
            count +=1.0;
            totalDepth += blockerDepth;
        }
    }
    if(noBlocker) {
        return 1.0;
    } else{
        return totalDepth/count;
    }
}

float PCF(int lightIndex, vec2 uv, float filterSize,float fragZ) {
  if(fragZ > 1.0 || fragZ < 0.0){
      return 0.0;
  }
  poissonDiskSamples(uv);
  vec2 texelSize = 1.0 / textureSize(lights[lightIndex].shadowMap, 0);
  float shadow = 0.0;
  float count = 0.0;
  for(int i = 0;i < NUM_SAMPLES;i++)
  {
    vec2 sampleCoord = uv + texelSize * vec2(filterSize) * poissonDisk[i];
    float blockerDepth = texture(lights[lightIndex].shadowMap, sampleCoord).r;
    shadow += ((fragZ- 0.001) > blockerDepth ? 1.0 : 0.0);
    count += 1.0;
  }
  return shadow/count;
}

float PCSS(int lightIndex, vec4 fragPosLightSpace){

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    float fragZ = projCoords.z;
    vec2 uv = projCoords.xy;

    float avgDepth = PCSSGetAvgDepth(lightIndex, fragPosLightSpace, uv, fragZ);
    
    vec2 texelSize = 1.0 / textureSize(lights[lightIndex].shadowMap, 0);
    float lightLength = 0.8/texelSize.x;

    float filterSize = max(fragZ-avgDepth,0.0)/avgDepth*lightLength;

    return PCF(lightIndex, uv, filterSize, fragZ);
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

vec3 PointLight(int lightIndex){
    //vec4 FragPosLightSpace =  texture(gFragPosLightSpace, TexCoords);
	vec3 WorldPos = texture(gPosition, TexCoords).rgb;
    vec4 FragPosLightSpace = lights[lightIndex].projMat*lights[lightIndex].viewMat*inverse(view)*vec4(WorldPos,1.0);
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    float ao = texture(texSSAO, TexCoords).r;
    bool isPBR = abs(texture(gExtra, TexCoords).r - 1.0)<0.01;
    float metallic = texture(gExtra, TexCoords).g;
    float roughness = texture(gExtra, TexCoords).b;
    vec3 lightPos = (view*vec4(lights[lightIndex].position,1.0)).xyz;

    vec3 N = normalize(Normal); 
    vec3 V = normalize(-WorldPos);//世界坐标到视线的向量
    vec3 Lo = vec3(0.0);
    vec3 L = normalize(lightPos - WorldPos);//世界坐标到光的向量
    vec3 H = normalize(V + L);
    //float distance    = length(lightPos - WorldPos);
    float distance = 1.0;
    float attenuation = 1.0 / (distance * distance); //todo:现在只有一个阳光，假设是太阳光，应该用平行光
    vec3 radiance     = lights[lightIndex].intensity * attenuation; 

    //albedo = pow(albedo, vec3(2.2));

    if(isPBR){
        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        vec3 F  = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        float NDF = DistributionGGX(N, H, roughness);       
        float G   = GeometrySmith(N, V, L, roughness); 
        vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
        vec3 specular = nominator / denominator;  

        float shadow = PCSS(lightIndex, FragPosLightSpace); 

        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1-shadow);

        //vec3 ambient = vec3(0.0005) * albedo * ao;
        vec3 color = Lo;  
        //vec3 color   =  vec3(F0 * envBRDF.x + envBRDF.y);
        return color;
    }
    
    else{
        vec3 lightIntensity = radiance;
        vec3 kd = albedo;
        vec3 diffuse = kd * max(0.0, dot(N, L)) * lightIntensity / distance;

        vec3 ks = vec3(0.7937, 0.7937, 0.7937);
        vec3 halfV = normalize(V + L);
        float spec = pow(max(dot(halfV, N), 0.0), 150);
        vec3 specular = ks * spec * lightIntensity / distance;

        float shadow = PCSS(lightIndex, FragPosLightSpace);
        vec3 lighting = (1.0 - shadow) * (diffuse + specular) * 0.3;  //和pbr放在一起使用一样的光照有点太亮了，原因未知，乘个系数
        return lighting;
    }
}

vec3 EnvLight(){
    //vec4 FragPosLightSpace =  texture(gFragPosLightSpace, TexCoords);
	vec3 WorldPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    float ao = texture(texSSAO, TexCoords).r;
    bool isPBR = abs(texture(gExtra, TexCoords).r - 1.0)<0.01;
    float metallic = texture(gExtra, TexCoords).g;
    float roughness = texture(gExtra, TexCoords).b;

    vec3 N = normalize(Normal); 
    vec3 V = normalize(-WorldPos);//世界坐标到视线的向量

    //albedo = pow(albedo, vec3(2.2));

    if(isPBR){
        vec3 F0 = mix(vec3(0.04), albedo, metallic);
        vec3 F  = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        //漫反射环境光 1-F * 1-m * c * L（PI已经处理过了）* ao

        vec3 ambientDiffuse = (1 - F) * (1-metallic) * texture(irradianceMap, N).rgb * ao * albedo;

        //镜面环境光
        vec3 R = reflect(-V, N);   
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 envLight = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;  
        vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 envSpecular = envLight * (F0 * envBRDF.x + envBRDF.y) * ao;

        vec3 color = ambientDiffuse  + envSpecular;  
        return color;
    }
    
    else{
        vec3 ambLightIntensity = vec3(100.0,100.0,100.0);
        vec3 ka = vec3(0.0005,0.0005,0.0005);
        vec3 ambient = ka * ambLightIntensity * ao;
        return ambient;
    }
}

void main()
{
    vec3 color = vec3(0.0,0.0,0.0);
    for(int i = 0;i<lightNum; i++){
        color += PointLight(i);
    }
    //处理环境光照
    color += EnvLight();

    color = color / (color + vec3(1.0));//色调映射

    //vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    //albedo = pow(albedo, vec3(1.0/2.2)); 
    //FragColor = vec4(pow(color, vec3(1.0/2.2)),1.0); 
    FragColor = vec4(color,1.0); 
}