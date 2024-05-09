#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
    vec4 position;
    vec3 Normal;
vec3 fragPos;
vec3 LightPos[2];
mat4 model;
} gs_in[];


out vec2 TexCoords; 
out vec3 Normal;
out vec3 fragPos;
out vec3 LightPos[2];


uniform mat4 projection;
uniform mat4 view;

vec4 explode(vec4 position, vec3 normal)
{
    vec3 direction = normal*0.0; 
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

    vec3 normal = GetNormal();
    vec4 pos;

    pos = gs_in[0].position;
    gl_Position = projection * view * gs_in[0].model * explode(pos, normal);
    TexCoords = gs_in[0].texCoords;
    Normal = gs_in[0].Normal;
    fragPos = gs_in[0].fragPos;
    LightPos[0] = gs_in[0].LightPos[0];
        LightPos[1] = gs_in[0].LightPos[1];
    EmitVertex();

    pos = gs_in[1].position;
    gl_Position =projection * view * gs_in[1].model *explode(pos, normal);
    TexCoords = gs_in[1].texCoords;
    Normal = gs_in[1].Normal;
    fragPos = gs_in[1].fragPos;
    LightPos[0] = gs_in[1].LightPos[0];
    LightPos[1] = gs_in[1].LightPos[1];
    EmitVertex();

    pos = gs_in[2].position;
    gl_Position = projection * view * gs_in[2].model * explode(pos, normal);
    TexCoords = gs_in[2].texCoords;
        Normal = gs_in[2].Normal;
    fragPos = gs_in[2].fragPos;
    LightPos[0] = gs_in[2].LightPos[0];
        LightPos[1] = gs_in[2].LightPos[1];
    EmitVertex();
    EndPrimitive();
}