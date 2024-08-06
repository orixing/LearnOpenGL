#include "FrameBuffer.h"
#include <glad/glad.h> 
#include <iostream> 
#include "GlobalConst.h"

FrameBuffer::~FrameBuffer() {}
FrameBuffer::FrameBuffer(unsigned int id, std::vector<Texture*>& colorTextures, Texture* depthTexture) : id(id) {
    allTextures.insert(allTextures.end(), colorTextures.begin(), colorTextures.end());
    if (depthTexture != nullptr) allTextures.push_back(depthTexture);
}

FrameBuffer::Builder& FrameBuffer::Builder::CreateRenderbuffer()
{
    //创建渲染缓冲组件
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, GlobalConst::ScreenWidth, GlobalConst::ScreenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    this->renderBuffer = new RenderBuffer(rbo);
	return *this;
}

FrameBuffer* FrameBuffer::Builder::Build()
{
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    std::vector<unsigned int> attachments;

    for (int i = 0;i < colorTextures.size();i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTextures[i]->id, 0);
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);

    }
    if (colorTextures.size() > 0) {
        glDrawBuffers(colorTextures.size(), attachments.data());
    }

    if (colorTextures.size() == 0 && !needColorTexture) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (depthTexture != nullptr) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->id, 0);
    }

    if (renderBuffer != nullptr) {
        // 之后同样添加渲染缓冲对象(Render Buffer Object)为深度缓冲(Depth Buffer)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->id);
    }

    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FrameBuffer not complete!" << std::endl;
        return nullptr;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return new FrameBuffer(fbo, colorTextures, depthTexture);

}

Texture* FrameBuffer::GetTexture(const std::string& name) {
    for (size_t i = 0; i < allTextures.size(); i++)
    {
        if (allTextures[i]->name == name) {
            return allTextures[i];
        }
    }
    return nullptr;
}

RenderBuffer::~RenderBuffer() {}
RenderBuffer::RenderBuffer(unsigned int id) :id(id) {}
