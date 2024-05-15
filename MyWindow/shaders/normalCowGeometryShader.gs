#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
 vec2 TexCoords;
 vec3 fragPos;
 vec3 LightPos;
 vec3 Normal;
 vec4 FragPosLightSpace;
 vec4 position;
} gs_in[];


out vec2 TexCoords; 
out vec3 fragPos;
out vec3 LightPos;
out vec4 FragPosLightSpace;
out mat3 TBN;
out vec3 T;
out vec3 B;
out vec3 N;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main() {    

    vec3 pos1 = vec3(gs_in[0].position);
    vec3 pos2 = vec3(gs_in[1].position);
    vec3 pos3 = vec3(gs_in[2].position);
    vec3 edge1 = pos2 - pos1;
    vec3 edge2 = pos3 - pos1;
    vec2 deltaUV1 = gs_in[1].TexCoords - gs_in[0].TexCoords;
    vec2 deltaUV2 = gs_in[2].TexCoords - gs_in[0].TexCoords;

    float f = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vec3 tangent1,bitangent1;

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = normalize(bitangent1);  


    gl_Position = projection * view * model * gs_in[0].position;
    TexCoords = gs_in[0].TexCoords;
    fragPos = gs_in[0].fragPos;
    LightPos= gs_in[0].LightPos;
    Normal = gs_in[0].Normal;
    FragPosLightSpace = gs_in[0].FragPosLightSpace;

    T = mat3(transpose(inverse(view * model))) * tangent1;
    B = mat3(transpose(inverse(view * model))) * bitangent1;
    //T = normalize(vec3(view * model * vec4(tangent1, 0.0)));
    N = mat3(transpose(inverse(view * model))) * gs_in[0].Normal;
    //N = normalize(vec3(view * model * vec4(0.0,0.0,1.0, 0.0)));
    //N = normalize(vec3(view * model * vec4(gs_in[0].Normal,0.0)));
    //N = mat3(transpose(inverse(view * model))) * vec3(0.0,0.0,1.0);
    T = normalize(T - dot(T, N) * N);
    B = normalize(B - dot(B, N) * N - dot(B, T) * T);
    TBN = mat3(T, B, N);

    EmitVertex();
     
    gl_Position = projection * view * model * gs_in[1].position;
    TexCoords = gs_in[1].TexCoords;
    fragPos = gs_in[1].fragPos;
    LightPos = gs_in[1].LightPos;
        Normal = gs_in[1].Normal;
    FragPosLightSpace = gs_in[1].FragPosLightSpace;

    T = mat3(transpose(inverse(view * model))) * tangent1;
    B = mat3(transpose(inverse(view * model))) * bitangent1;
    //T = normalize(vec3(view * model * vec4(tangent1, 0.0)));
    N = mat3(transpose(inverse(view * model))) * gs_in[1].Normal;
    //N = normalize(vec3(view * model * vec4(gs_in[1].Normal,0.0)));
    //N = mat3(transpose(inverse(view * model))) * vec3(0.0,0.0,1.0);
    T = normalize(T - dot(T, N) * N);
    B = normalize(B - dot(B, N) * N - dot(B, T) * T);
    TBN = mat3(T, B, N);

    EmitVertex();


    gl_Position = projection * view * model  * gs_in[2].position;
    TexCoords = gs_in[2].TexCoords;
    fragPos = gs_in[2].fragPos;
    LightPos = gs_in[2].LightPos;
        Normal = gs_in[2].Normal;
    FragPosLightSpace = gs_in[2].FragPosLightSpace;

    T = mat3(transpose(inverse(view * model))) * tangent1;
        B = mat3(transpose(inverse(view * model))) * bitangent1;
    //N = normalize(vec3(view * model * vec4(0.0,0.0,1.0, 0.0)));
    //N = mat3(transpose(inverse(view * model))) * vec3(0.0,0.0,1.0);
    N = mat3(transpose(inverse(view * model))) * gs_in[2].Normal;
    //N = normalize(vec3(view * model * vec4(gs_in[2].Normal,0.0)));
    T = normalize(T - dot(T, N) * N);
    B = normalize(B - dot(B, N) * N - dot(B, T) * T);
    TBN = mat3(T, B, N);


    EmitVertex();
    EndPrimitive();
}