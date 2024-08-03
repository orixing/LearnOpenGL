#include "FrameBuffer.h"
#include <glad/glad.h> 
#include <iostream> 
#include "GlobalConst.h"

FrameBuffer::~FrameBuffer() {}
FrameBuffer::FrameBuffer(unsigned int id,std::vector<Texture*>& textures, RenderBuffer* renderBuffer) : id(id), textures(textures), renderBuffer(renderBuffer) {}

FrameBuffer::Builder& FrameBuffer::Builder::BindTexture(Texture* texture) {
	textures.push_back(texture);
	return *this;

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

    for (int i = 0;i < textures.size();i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i]->id, 0);
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);

    }
    if (textures.size() > 0) {
        glDrawBuffers(textures.size(), attachments.data());
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
    return new FrameBuffer(fbo, textures, renderBuffer);

}

Texture* FrameBuffer::GetTexture(const std::string& name) {
    for (size_t i = 0; i < textures.size(); i++)
    {
        if (textures[i]->name == name) {
            return textures[i];
        }
    }
    return nullptr;
}

RenderBuffer::~RenderBuffer() {}
RenderBuffer::RenderBuffer(unsigned int id) :id(id) {}
