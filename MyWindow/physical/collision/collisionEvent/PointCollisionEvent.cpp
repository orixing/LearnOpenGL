#include "PointCollisionEvent.h"

PointCollisionEvent::PointCollisionEvent(PhysicalComponent* obj1, PhysicalComponent* obj2, glm::vec3 point)
	: CollisionEvent(obj1,obj2), collisionPoint(point){}