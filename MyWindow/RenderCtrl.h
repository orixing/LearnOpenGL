#pragma once
#include <vector>
#include "GameObj.h"
#include <glad/glad.h> 
#include "WindowContent.h"
#include "FrameBuffer.h"
class RenderCtrl
{
public:
    RenderCtrl(const RenderCtrl&) = delete;
    RenderCtrl& operator=(const RenderCtrl&) = delete;

    // 获取单例实例的方法
    static RenderCtrl& getInstance();

    void DoGPass(WindowContent* content);
    void DoGBorderPass(WindowContent* content);
    void DoSSAOPass(WindowContent* content);
    void DoSSAOBlurPass(WindowContent* content);
    void DoShadowPass(WindowContent* content);

    std::vector<glm::vec3> SSAOKernel;
    Texture* SSAONoiseTex;
    FrameBuffer* GBuffer, * SSAOFBO, * SSAOBlurFBO;

    unsigned int screenVAO;

    void Render(WindowContent* content);

private:
    Shader* gPassShader = new Shader("../../MyWindow/rawShaders/GVertexShader.vs", "../../MyWindow/rawShaders/GFregmentShader.fs");
    Shader* gBorderShader = new Shader("../../MyWindow/rawShaders/GBorderVertexShader.vs", "../../MyWindow/rawShaders/GBorderFragmentShader.fs");
    Shader* SSAOShader = new Shader("../../MyWindow/rawShaders/SSAOVertexShader.vs", "../../MyWindow/rawShaders/SSAOFregmentShader.fs");
    Shader* SSAOBlurShader = new Shader("../../MyWindow/rawShaders/SSAOVertexShader.vs", "../../MyWindow/rawShaders/SSAOBlurFragmentShader.fs");
    Shader* shadowShader = new Shader("../../MyWindow/rawShaders/shadowVertexShader.vs", "../../MyWindow/rawShaders/shadowFragmentShader.fs");
    // 私有构造函数和析构函数
    RenderCtrl();
    ~RenderCtrl();
};

