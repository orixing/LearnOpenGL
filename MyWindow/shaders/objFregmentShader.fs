#version 330 core
out vec4 FragColor;
in vec3 normal;
in vec3 fragViewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 freg2Light = normalize(light.position - fragViewPos);
    vec3 n = normalize(normal);

    vec3 diffuse = material.diffuse * max(0.0, dot(n, freg2Light)) * light.diffuse;

    vec3 viewPos = vec3(0.0,0.0,0.0);
    vec3 freg2View = normalize(viewPos - fragViewPos);
    vec3 reflectDir = reflect(-freg2Light, n);
    float spec = pow(max(dot(freg2View, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    FragColor = vec4((ambient + diffuse + specular), 1.0);
};