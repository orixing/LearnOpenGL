#pragma once
#include "Texture.h"
#include <vector>
#include<initializer_list>
class FrameBuffer
{
public:
	~FrameBuffer();
	FrameBuffer();

	unsigned int id;


	void BindTexture(Texture* texture,GLenum attachment, GLenum texTarget = GL_TEXTURE_2D, GLint mipLevel= 0);
	void AddRenderbuffer(GLenum attachment, GLenum internalFormat, int width, int height);
	void NoColorTexture();
	void SetDrawBuffers(std::initializer_list<GLenum> attachments);

	Texture* GetTexture(const std::string& name);

private:
	std::vector<Texture*> allTextures;//所有的颜色附件
};