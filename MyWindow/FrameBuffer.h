#pragma once
#include "RenderBuffer.h";
#include "Texture.h";
class FrameBuffer
{
public:
	unsigned int id;
	~FrameBuffer();
	FrameBuffer(int id);

	void BindTexture(Texture* texture);
	void BindRenderbuffer(RenderBuffer* renderBuffer);
};

