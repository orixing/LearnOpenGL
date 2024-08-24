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


class Vertex {
public:
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    static vector<Vertex> genVertexByRawData(vector<float>& vertexData);
};

class Mesh {
public:
    /*  ��������  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    //todo:��ȥ��
    vector<std::pair<glm::vec3, glm::vec3>> lines;
    /*  ����  */
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);
    void Draw();
private:
    /*  ��Ⱦ����  */
    unsigned int VAO, VBO, EBO;
    /*  ����  */
    void setupMesh();
};