#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "Shader.h"
using std::string;
using std::vector;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    /*  ��������  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    /*  ����  */
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);
    void Draw();
private:
    /*  ��Ⱦ����  */
    unsigned int VAO, VBO, EBO;
    /*  ����  */
    void setupMesh();
};
