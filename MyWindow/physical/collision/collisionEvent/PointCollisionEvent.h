#pragma once
#include "CollisionEvent.h"
#include <glm/glm/glm.hpp>
class PointCollisionEvent : public CollisionEvent
{
public:
	glm::vec3 collisionPoint;
	PointCollisionEvent(PhysicalComponent* obj1, PhysicalComponent* obj2, glm::vec3 point);
};

