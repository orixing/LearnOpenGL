#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform samplerCube skybox;
uniform mat4 view;
void main()
{             
    vec3 I = normalize(Position);
    vec3 R = reflect(I, normalize(Normal));
    //FragColor = vec4(texture(skybox, vec3(R.x,-R.y,R.z)).rgb, 1.0);
    vec3 mr = vec3(inverse(view) * vec4(R,0.0));
    FragColor = vec4(texture(skybox, vec3(mr.x,-mr.y,mr.z)).rgb, 1.0);
    //FragColor = vec4(normalize(Normal), 1.0);
}