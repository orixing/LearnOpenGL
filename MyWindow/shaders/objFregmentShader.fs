#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 fragPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D spotTex;
in vec3 LightPos[2];

void main()
{
    vec3 color;
    for(int i = 0;i<2;i++){
        vec3 ambLightIntensity = vec3(1.0,1.0,1.0);
        vec3 ka = vec3(0.05,0.05,0.05);
        vec3 ambient = ka * ambLightIntensity;

        vec3 lightIntensity = vec3(8.0,8.0,8.0);
        vec3 kd = vec3(texture(spotTex, TexCoords));
        float distance = length(LightPos[i]-fragPos);

        vec3 lightDir = normalize(LightPos[i] - fragPos);
        vec3 n = normalize(Normal);
        vec3 diffuse = kd * max(0.0, dot(n, lightDir)) * lightIntensity / distance / distance;

        vec3 ks = vec3(0.7937, 0.7937, 0.7937);
        vec3 viewPos = vec3(0.0,0.0,0.0);
        vec3 freg2View = normalize(viewPos - fragPos);
        vec3 halfV = normalize(freg2View + lightDir);
        float spec = pow(max(dot(halfV, n), 0.0), 50);
        vec3 specular = ks * spec * lightIntensity / distance / distance;

        color += ambient + diffuse + specular;
    }


    FragColor = vec4(color,1.0);
};