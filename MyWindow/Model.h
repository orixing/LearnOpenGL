#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include "Shader.h"
#include "Mesh.h"
#include "stb_image.h"

class Model
{
public:
    /*  ����   */
    Model(const char* path)
    {
        loadModel(path);
    }
    void Draw();
private:
    /*  ģ������  */
    vector<Mesh> meshes;
    string directory;
    /*  ����   */
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};