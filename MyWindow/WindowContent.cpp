#include "WindowContent.h"
#include "PBRMetalMaterial.h"
#include "CommonObj.h"
#include "RigidBody.h"
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

    model = new Model("../../MyWindow/spot/spot_triangulated_good.obj");
}
WindowContent::~WindowContent() {}

void WindowContent::ShootNewObj() {
    //创建一个随机的物体并发射
    Model* newModel = new Model(*model);
    PBRMetalMaterial* m = new PBRMetalMaterial();
    m->albedoTex = TextureCtrl::getInstance().getTexture(TextureEnum::CowAlbedoTex);
    CommonObj* cow = new CommonObj(newModel, m);
    cow->position = mainCamera->Position + mainCamera->Direc *2.0f;
    cow->scaleX = cow->scaleY = cow->scaleZ = 1.0f;
    RigidBody* physical = new RigidBody(cow);
    physical->v = 2.0f * mainCamera->Direc;
    physical->SetDynamic(true);
    cow->physical = physical;
    allObjs->push_back(cow);
}