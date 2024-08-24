#pragma once
#include "Mesh.h"
#include "BoundingBox.h"
#include <glm/glm/glm.hpp>
class IPhysical
{
public:
	IPhysical() {
		boundingBox = new BoundingBox();
	}

	virtual Mesh* GetMesh() = 0;
	virtual void UpdatePosition(glm::vec3 deltaX) = 0;
	virtual void UpdateRotation(glm::vec3 dw) = 0;
	virtual glm::vec3 GetPosition() = 0;
	virtual glm::quat GetRotation() = 0;
	virtual glm::vec3 GetRadiusVector(glm::vec3 localPos) = 0;
	virtual glm::vec3 GetScale() = 0;
	virtual glm::mat4 getModelMatrix() = 0;

	BoundingBox* boundingBox;
};

