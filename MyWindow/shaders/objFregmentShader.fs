#version 330 core
out vec4 FragColor;
in vec3 normal;
in vec3 fragViewPos;
uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 freg2Light = normalize(lightPos - fragViewPos);
    vec3 n = normalize(normal);

    vec3 diffuse = max(0.0, dot(n, freg2Light)) * lightColor;

    vec3 viewPos = vec3(0.0,0.0,0.0);
    vec3 freg2View = normalize(viewPos - fragViewPos);
    vec3 reflectDir = reflect(-freg2Light, n);
    float spec = pow(max(dot(freg2View, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightColor;

    FragColor = vec4((ambient + diffuse + specular) * objColor, 1.0);
};