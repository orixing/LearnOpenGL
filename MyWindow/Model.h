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
    Model(Mesh* mesh) {
        meshes.push_back(*mesh);
    }

    void Draw();

    vector<Mesh> meshes;//todo:ģ�ͼ������������⣬Ӧ����Obj�ļ�ֱ�Ӽ��س�Ƕ�׵�GameObj��ÿ��GameObj�ж�Ӧ��mesh��Ȼ��û��model��
    //ţģ��ֻ��һ��mesh���Ȳ�����Ƕ��
private:
    /*  ����   */
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};