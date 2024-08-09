#pragma once
#include <string>
#include "glad\glad.h"
#include "GlobalConst.h"
#include <vector>
#include <functional>
class Texture
{
public:
	unsigned int id;
	std::string name;
	~Texture();
	Texture(unsigned int id, const std::string& name);

	class Builder {
	public:
		Builder& LoadFromFile(const std::string& path);
		Builder& LoadFromHDRFile(const std::string& path);

		inline Builder& SetTarget(GLenum target) {
			this->target = target;
			return *this;
		}
		inline Builder& SetInternalFormat(GLint internalFormat) {
			this->internalFormat = internalFormat;
			return *this;
		}
		inline Builder& SetWidth(int width) {
			this->width = width;
			return *this;
		}
		inline Builder& SetHeight(int height) {
			this->height = height;
			return *this;
		}
		inline Builder& SetFormat(GLenum format) {
			this->format = format;
			return *this;
		}
		inline Builder& SetType(GLenum type) {
			this->type = type;
			return *this;
		}
		inline Builder& SetData(void* data) {
			this->data = data;
			return *this;
		}
		inline Builder& SetMinFilter(GLint minFilter) {
			this->minFilter = minFilter;
			return *this;
		}
		inline Builder& SetMagFilter(GLint magFilter) {
			this->magFilter = magFilter;
			return *this;
		}
		inline Builder& SetWrapS(GLint warpS) {
			this->wrapS = warpS;
			return *this;
		}
		inline Builder& SetWrapT(GLint warpT) {
			this->wrapT = warpT;
			return *this;
		}
		inline Builder& SetMipMap(bool needMipMap) {
			this->needMipMap = needMipMap;
			return *this;
		}

		inline Builder& SetName(const std::string name) {
			this->name = name;
			return *this;
		}

		inline Builder& SetBorder(GLfloat* border) {
			this->border = border;
			return *this;
		}

		Texture* Build();

	private:
		GLenum target = GL_TEXTURE_2D;
		GLint internalFormat = GL_RGBA16F;
		int width = GlobalConst::ScreenWidth;
		int height = GlobalConst::ScreenHeight;
		GLenum format = GL_RGBA;
		GLenum type = GL_FLOAT;
		void* data = nullptr;
		GLint minFilter = GL_NEAREST;
		GLint magFilter = GL_NEAREST;
		GLint wrapS = GL_CLAMP_TO_EDGE;
		GLint wrapT = GL_CLAMP_TO_EDGE;
		GLint wrapR = GL_CLAMP_TO_EDGE;
		bool needMipMap = false;
		std::string name;
		std::vector<std::function<void()>> afterBuild;
		GLfloat* border;
	};

};