#pragma once
#include <vector>
#include "GameObj.h"
#include <glad/glad.h> 
#include "WindowContent.h"
#include "FrameBuffer.h"
#include "ScreenObj.h"
class RenderCtrl
{
public:
    RenderCtrl(const RenderCtrl&) = delete;
    RenderCtrl& operator=(const RenderCtrl&) = delete;

    // 获取单例实例的方法
    static RenderCtrl& getInstance();

    WindowContent* curRenderContent;

    void Render(WindowContent* content);
    //todo:ShaderCtrl
    Shader* grassShader = new Shader("../../MyWindow/rawShaders/grassVertexShader.vs", "../../MyWindow/rawShaders/grassFragmentShader.fs");

private:
    Shader* gPassShader = new Shader("../../MyWindow/rawShaders/GVertexShader.vs", "../../MyWindow/rawShaders/GFregmentShader.fs");
    Shader* SSAOShader = new Shader("../../MyWindow/rawShaders/SSAOVertexShader.vs", "../../MyWindow/rawShaders/SSAOFregmentShader.fs");
    Shader* SSAOBlurShader = new Shader("../../MyWindow/rawShaders/SSAOVertexShader.vs", "../../MyWindow/rawShaders/SSAOBlurFragmentShader.fs");
    Shader* shadowShader = new Shader("../../MyWindow/rawShaders/shadowVertexShader.vs", "../../MyWindow/rawShaders/shadowFragmentShader.fs");
    Shader* PBRShader = new Shader("../../MyWindow/rawShaders/PBRVertexShader.vs", "../../MyWindow/rawShaders/PBRFragmentShader.fs");
    Shader* skyboxShader = new Shader ("../../MyWindow/rawShaders/skyboxVertexShader.vs", "../../MyWindow/rawShaders/skyboxFragmentShader.fs");
    Shader* borderShader = new Shader("../../MyWindow/rawShaders/GBorderVertexShader.vs", "../../MyWindow/rawShaders/borderRenderFragmentShader.fs");    
    Shader* screenShader = new Shader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/screenFragmentShader.fs");
    Shader* FXAAShader = new Shader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/FXAAFragmentShader.fs");
    // 私有构造函数和析构函数
    RenderCtrl();
    ~RenderCtrl();

    ScreenObj* screenObj = &ScreenObj::getInstance();

    std::vector<glm::vec3> SSAOKernel;

    void DoGPass();
    void DoSSAOPass();
    void DoSSAOBlurPass();
    void DoShadowPass();
    void DoPBRRenderPass();
    void RenderSkybox();
    void TransparentPass();
    void RenderBorder();
    void DrawToScreen();
};

