#pragma once
#include "Texture.h"
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
		inline Builder& BindColorTexture(Texture* texture) {
			colorTextures.push_back(texture);
			return *this;
		}

		inline Builder& BindDepthTexture(Texture* texture) {
			depthTexture = texture;
			return *this;
		}

		inline Builder& NeedColorTexture(bool needColorTexture) {
			this->needColorTexture = needColorTexture;
			return *this;
		}
		Builder& CreateRenderbuffer();

		FrameBuffer* Build();
	private:
		std::vector<Texture*> colorTextures;//所有待绑定的颜色附件
		Texture* depthTexture;
		bool needColorTexture = true;
		RenderBuffer* renderBuffer;//待绑定的渲染缓冲附件
	};


private:
	std::vector<Texture*> allTextures;//所有的颜色附件

	~FrameBuffer();
	FrameBuffer(unsigned int id,std::vector<Texture*>& colorTextures, Texture* depthTexture);
};