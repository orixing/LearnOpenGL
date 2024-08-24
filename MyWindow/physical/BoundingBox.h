#pragma once
#include <glm/glm/glm.hpp>
class BoundingBox
{
public:
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;

	void Clear();
	void AddOneVertex(glm::vec3 v);
	bool CollisionDetect(BoundingBox* other);
};
