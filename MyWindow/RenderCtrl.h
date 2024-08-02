#pragma once
#include <vector>
#include "GameObj.h"
#include <glad/glad.h> 
#include "WindowContent.h"
class RenderCtrl
{
public:
    RenderCtrl(const RenderCtrl&) = delete;
    RenderCtrl& operator=(const RenderCtrl&) = delete;

    // ��ȡ����ʵ���ķ���
    static RenderCtrl& getInstance();

    void DoGPass(WindowContent* content);
    void DoGPassBorder(WindowContent* content);

    GLuint gBuffer;
    GLuint gPositionDepth, gNormal, gAlbedoSpec, gFragPosInLight, gBorder, gExtra;
    Texture* gFragColor;


    void Render(WindowContent* content);

private:
    Shader* gPassShader = new Shader("../../MyWindow/rawShaders/GVertexShader.vs", "../../MyWindow/rawShaders/GFregmentShader.fs");
    Shader* gBorderShader = new Shader("../../MyWindow/rawShaders/GBorderVertexShader.vs", "../../MyWindow/rawShaders/GBorderFragmentShader.fs");
    // ˽�й��캯������������
    RenderCtrl();
    ~RenderCtrl();
};

