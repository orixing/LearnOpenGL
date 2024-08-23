#include "FrameBuffer.h"
#include <glad/glad.h> 
#include <iostream> 
#include "GlobalConst.h"
#include<vector>
FrameBuffer::~FrameBuffer() {}
FrameBuffer::FrameBuffer(){
    glGenFramebuffers(1, &id);
}

void FrameBuffer::BindTexture(Texture* texture, GLenum attachment, GLenum texTarget, GLint mipLevel) {
    allTextures.push_back(texture);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texTarget, texture->id, mipLevel);
}
void FrameBuffer::AddRenderbuffer(GLenum attachment, GLenum internalFormat, int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
}

void FrameBuffer::NoColorTexture(){
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void FrameBuffer::SetDrawBuffers(std::initializer_list<GLenum> attachments) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    std::vector<unsigned int> vector;

    for (GLenum e : attachments)
    {
        vector.push_back(e);
    }
    glDrawBuffers(vector.size(), vector.data());
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