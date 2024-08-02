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

    // ��ȡ����ʵ���ķ���
    static GPassCtrl& getInstance();

    void DoGPass(std::vector<GameObj*>* allObjs);

    GLuint gBuffer;
    GLuint gFragColor, gPositionDepth, gNormal, gAlbedoSpec, gFragPosInLight, gBorder, gExtra;
private:
    GPassShader* gPassShader = new GPassShader();
    // ˽�й��캯������������
    GPassCtrl();
    ~GPassCtrl();
};

