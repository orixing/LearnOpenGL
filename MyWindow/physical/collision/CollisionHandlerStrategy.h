#pragma once
#include "RigidBody.h"
#include "StaticEntity.h"
class CollisionHandlerStrategy
{
public:
	static void RigidStaticCollision(RigidBody* rigid, StaticEntity* staticEntity, CollisionEvent* event);
};

