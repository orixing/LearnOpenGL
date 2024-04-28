#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 fragPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightPos;

void main()
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 n = normalize(Normal);
    vec3 diffuse = vec3(texture(texture_diffuse1, TexCoords)) * max(0.0, dot(n, lightDir)) * vec3(1.0,1.0,1.0);

    //vec3 reflectDir = reflect(-lightDir, n);
    //float spec = pow(max(0.0, dot(reflectDir, normalize(-fragPos))),64);
    //vec3 specular = vec3(texture(texture_specular1, TexCoords)) * spec * vec3(10.0,10.0,10.0);
    vec3 viewPos = vec3(0.0,0.0,0.0);
    vec3 freg2View = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, n);
    float distance = length(lightPos-fragPos);
    float spec = pow(max(dot(freg2View, reflectDir), 0.0), 32);
    vec3 specular = vec3(texture(texture_specular1, TexCoords)) * spec * vec3(1.0,1.0,1.0);


    FragColor = vec4(diffuse+specular,1.0);
    //FragColor = vec4(max(dot(freg2View, reflectDir),0.0)*100,0.0,0.0,1.0);
    //FragColor = vec4(fragPos.z,0.0,0.0,1.0);
};