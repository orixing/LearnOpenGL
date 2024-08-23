#include "WindowContent.h"


WindowContent::WindowContent(Camera* c) :mainCamera(c) {
	allObjs = new std::vector<GameObj*>();
	allLights = new std::vector<Light*>();

    GBuffer = new FrameBuffer();

    GBuffer->BindTexture(Texture::Builder().SetName("gPosition").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);// - 世界坐标位置
    GBuffer->BindTexture(Texture::Builder().SetName("gNormal").Build(), GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D);// - 法线
    GBuffer->BindTexture(Texture::Builder().SetName("gAlbedo").Build(), GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D);//albedo颜色
    GBuffer->BindTexture(Texture::Builder().SetName("gExtra").Build(), GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D);//材质信息
    GBuffer->AddRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    GBuffer->SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 });

    SSAOFBO = new FrameBuffer();
    SSAOFBO->BindTexture(Texture::Builder().SetName("ssaoTex").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);

    SSAOBlurFBO = new FrameBuffer();
    SSAOBlurFBO->BindTexture(Texture::Builder().SetName("ssaoBlurTex").Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);

    postProcessingFBO = new FrameBuffer();
    postProcessingFBO->BindTexture(Texture::Builder().SetName("screenTex").SetInternalFormat(GL_RGB).SetFormat(GL_RGB).SetType(GL_UNSIGNED_BYTE).Build(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);
    postProcessingFBO->AddRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
}
WindowContent::~WindowContent() {}