#pragma once
#include <vector>
#include "GameObj.h"
#include <glad/glad.h> 
#include "GPassShader.h"

class GPassCtrl
{
public:
    GPassCtrl(const GPassCtrl&) = delete;
    GPassCtrl& operator=(const GPassCtrl&) = delete;

    // 获取单例实例的方法
    static GPassCtrl& getInstance();

    void DoGPass(std::vector<GameObj*>* allObjs);

    GLuint gBuffer;
    GLuint gFragColor, gPositionDepth, gNormal, gAlbedoSpec, gFragPosInLight, gBorder, gExtra;
private:
    GPassShader* gPassShader = new GPassShader();
    // 私有构造函数和析构函数
    GPassCtrl();
    ~GPassCtrl();
};

