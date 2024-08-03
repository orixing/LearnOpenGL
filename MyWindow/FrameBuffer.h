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
		std::vector<Texture*> textures;//所有待绑定的颜色附件
		RenderBuffer* renderBuffer;//待绑定的渲染缓冲附件
	};


private:
	std::vector<Texture*> textures;//所有的颜色附件
	RenderBuffer* renderBuffer;//渲染缓冲附件

	~FrameBuffer();
	FrameBuffer(unsigned int id,std::vector<Texture*>& textures,RenderBuffer* renderBuffer);
};