#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D groundTex;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec3 Normal;
in vec3 LightPos;
uniform sampler2D shadowMap;
uniform mat4 lightInverseProj;

const float PI = 3.141592653589793;
const float PI2 = 6.283185307179586;
const int NUM_SAMPLES = 200;



vec2 poissonDisk[NUM_SAMPLES];

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
float PCSSGetAvgDepth(vec4 fragPosLightSpace, vec2 uv,float fragZ){
    poissonDiskSamples(uv);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    vec4 fragPosLightView = lightInverseProj * fragPosLightSpace;
    float radiusF = fragPosLightView.z*0.8/(0.1+fragPosLightView.z)/texelSize.x;

    int radius = int(radiusF)+1;
    float count =0.0;
    float totalDepth =0.0;
    bool noBlocker = true;
    for(int i = 0;i<NUM_SAMPLES;i++){
        vec2 sampleCoord = uv + texelSize * vec2(radius) * poissonDisk[i];
        float blockerDepth = texture(shadowMap, sampleCoord).r;
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

float PCF(vec2 uv, float filterSize,float fragZ) {
  if(fragZ > 1.0 || fragZ < 0.0){
      return 0.0;
  }
  poissonDiskSamples(uv);
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  float shadow = 0.0;
  float count = 0.0;
  for(int i = 0;i < NUM_SAMPLES;i++)
  {
    vec2 sampleCoord = uv + texelSize * vec2(filterSize) * poissonDisk[i];
    float blockerDepth = texture(shadowMap, sampleCoord).r;
    shadow += ((fragZ- 0.001) > blockerDepth ? 1.0 : 0.0);
    count += 1.0;
  }
  return shadow/count;
}

float PCSS(vec4 fragPosLightSpace){

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    float fragZ = projCoords.z;
    vec2 uv = projCoords.xy;

    float avgDepth = PCSSGetAvgDepth(fragPosLightSpace, uv, fragZ);
    
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float lightLength = 0.8/texelSize.x;

    float filterSize = max(fragZ-avgDepth,0.0)/avgDepth*lightLength;

    return PCF(uv, filterSize, fragZ);
}

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


void main()
{
    vec4 color = texture(groundTex, TexCoords);
    //float shadow = ShadowCalculation(FragPosLightSpace,normalize(Normal), normalize(LightPos-FragPos));
    float shadow = PCSS(FragPosLightSpace);
    //shadow = 0.0;
    color = color * (1-shadow);

    FragColor = vec4(color.xyz,1.0);
    //FragColor = vec4(normalize(Normal),1.0);
}