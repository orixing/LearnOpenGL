#pragma once
#include "PhysicalComponent.h"
#include <glm/glm/glm.hpp>
class DCDStrategy
{
public:
	//�������Mesh�Ƿ����ཻ
	static bool MeshDCD(PhysicalComponent* obj, PhysicalComponent* other, CollisionEvent** event);
};

