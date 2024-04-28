#version 330 core
out vec4 FragColor;
in vec3 normal;
in vec3 fragPos;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
uniform Material material;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;

    vec3 direction;
    float cutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform SpotLight spotLight;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight spotLight,vec3 n,vec3 fragPos,vec3 viewDir);    

void main()
{
     // ����
    vec3 n = normalize(normal);
    vec3 viewDir = normalize(-fragPos);

    // ��һ�׶Σ��������
    vec3 result = CalcDirLight(dirLight, n, viewDir);
    // �ڶ��׶Σ����Դ
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(pointLights[i], n, fragPos, viewDir);  
    }   
    // �����׶Σ��۹�
    result += CalcSpotLight(spotLight, n, fragPos, viewDir);    

    FragColor = vec4(result, 1.0);
};


vec3 CalcSpotLight(SpotLight spotLight,vec3 n,vec3 fragPos,vec3 viewDir){
    vec3 color;
    vec3 freg2Light = normalize(spotLight.position - fragPos);
    float theta = dot(normalize(spotLight.direction), -freg2Light);
    if(theta > spotLight.cutoff){
        vec3 ambient = spotLight.ambient * vec3(texture(material.diffuse, TexCoords));

        vec3 n = normalize(normal);
        vec3 diffuse = vec3(texture(material.diffuse, TexCoords)) * max(0.0, dot(n, freg2Light)) * spotLight.diffuse;

        vec3 viewPos = vec3(0.0,0.0,0.0);
        vec3 freg2View = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect(-freg2Light, n);
        float spec = pow(max(dot(freg2View, reflectDir), 0.0), material.shininess);
        vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * spotLight.specular;
        color = ambient + diffuse + specular;
    } else {
        color = spotLight.ambient * vec3(texture(material.diffuse, TexCoords));
    }

    return color;
}



vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥��
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}