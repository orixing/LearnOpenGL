#pragma once
#include <glm/glm/glm.hpp>
#include "FrameBuffer.h"
class Light
{
public:
	Light(glm::vec3 lightPos);
	~Light();
	glm::mat4 GetLightSpaceMatrix();
	glm::mat4 GetLightProjection();

	FrameBuffer* depthMapFBO;

	static const int ShadowMapWidth = 1024;
	static const int ShadowMapHeight = 1024;
	glm::vec3 lightPos;
private:
};

