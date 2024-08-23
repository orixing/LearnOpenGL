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
    /*  函数   */
    Model(const char* path)
    {
        loadModel(path);
    }
    Model(Mesh* mesh) {
        meshes.push_back(*mesh);
    }

    void Draw();

    vector<Mesh> meshes;//todo:模型加载这里有问题，应该是Obj文件直接加载成嵌套的GameObj，每个GameObj有对应的mesh，然后没有model类
    //牛模型只有一个mesh，先不处理嵌套
private:
    /*  函数   */
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};