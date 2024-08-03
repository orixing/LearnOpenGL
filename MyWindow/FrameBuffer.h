#pragma once
#include "Texture.h";
#include <vector>

class RenderBuffer
{
public:
	unsigned int id;
	~RenderBuffer();
	RenderBuffer(unsigned int id);
};

class FrameBuffer
{
public:
	unsigned int id;

	Texture* GetTexture(const std::string& name);

	class Builder {
	public:
		Builder& BindTexture(Texture* texture);
		Builder& CreateRenderbuffer();

		FrameBuffer* Build();
	private:
		std::vector<Texture*> textures;//���д��󶨵���ɫ����
		RenderBuffer* renderBuffer;//���󶨵���Ⱦ���帽��
	};


private:
	std::vector<Texture*> textures;//���е���ɫ����
	RenderBuffer* renderBuffer;//��Ⱦ���帽��

	~FrameBuffer();
	FrameBuffer(unsigned int id,std::vector<Texture*>& textures,RenderBuffer* renderBuffer);
};