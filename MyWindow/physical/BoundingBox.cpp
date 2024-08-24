#include "BoundingBox.h"
#include <limits>
#include <algorithm>
void BoundingBox::Clear() {
	minX = FLT_MAX;
	maxX = -FLT_MAX;
	minY = FLT_MAX;
	maxY = -FLT_MAX;
	minZ = FLT_MAX;
	maxZ = -FLT_MAX;
}
void BoundingBox::AddOneVertex(glm::vec3 v) {
	minX = std::min(v.x, minX);
	minY = std::min(v.y, minY);
	minZ = std::min(v.z, minZ);
	maxX = std::max(v.x, maxX);
	maxY = std::max(v.y, maxY);
	maxZ = std::max(v.z, maxZ);
}

bool BoundingBox::CollisionDetect(BoundingBox* other) {
	return (other->minX < maxX && minX < other->maxX) && (other->minY < maxY && minY < other->maxY) && (other->minZ < maxZ && minZ < other->maxZ);
}