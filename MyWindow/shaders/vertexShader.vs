#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

out vec3 fragColor;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   vec3 fragViewPos = vec3(view * model * vec4(aPos,1.0));
    
    vec3 normal = mat3(transpose(inverse(view * model))) * aNormal;

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

    fragColor = ambient + diffuse + specular;
}