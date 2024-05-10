#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
    vec4 position;
    vec3 Normal;
vec3 fragPos;
vec3 LightPos;
vec4 FragPosLightSpace;
} gs_in[];


out vec2 TexCoords; 
out vec3 Normal;
out vec3 fragPos;
out vec3 LightPos;
out vec4 FragPosLightSpace;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

vec4 explode(vec4 position, vec3 normal)
{
    vec3 direction = normal*0; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal() {
    vec3 pos0 = vec3(gs_in[0].position);
    vec3 pos1 = vec3(gs_in[1].position);
    vec3 pos2 = vec3(gs_in[2].position);
    vec3 a = pos0 - pos1;
    vec3 b = pos2 - pos1;
    return normalize(cross(b, a));
}

void main() {    

    mat4 mvp = projection * view * model;
    vec3 normal = GetNormal();
    vec4 pos;

    pos = gs_in[0].position;
    gl_Position = mvp * explode(pos, normal);
    TexCoords = gs_in[0].texCoords;
    Normal = gs_in[0].Normal;
    fragPos = gs_in[0].fragPos;
    LightPos= gs_in[0].LightPos;
    FragPosLightSpace = gs_in[0].FragPosLightSpace;
    EmitVertex();

    pos = gs_in[1].position;
    gl_Position = mvp * explode(pos, normal);
    TexCoords = gs_in[1].texCoords;
    Normal = gs_in[1].Normal;
    fragPos = gs_in[1].fragPos;
    LightPos = gs_in[1].LightPos;
    FragPosLightSpace = gs_in[1].FragPosLightSpace;
    EmitVertex();

    pos = gs_in[2].position;
    gl_Position = mvp * explode(pos, normal);
    TexCoords = gs_in[2].texCoords;
        Normal = gs_in[2].Normal;
    fragPos = gs_in[2].fragPos;
    LightPos = gs_in[2].LightPos;
        FragPosLightSpace = gs_in[2].FragPosLightSpace;
    EmitVertex();
    EndPrimitive();
}