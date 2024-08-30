#pragma once
#include "PhysicalComponent.h"
#include <glm/glm/glm.hpp>
class DCDStrategy
{
public:
	//检测两个Mesh是否有相交
	static bool MeshDCD(PhysicalComponent* obj, PhysicalComponent* other, CollisionEvent** event);
};

