#pragma once
#include "PhysicalComponent.h";
class StaticEntity : public PhysicalComponent
{
public:
	StaticEntity(IPhysical* obj);

	bool DiscreteCollisionDetect(PhysicalComponent* other, CollisionEvent** event) override;
	bool CollisionDetectImpl(RigidBody* other, CollisionEvent** event) override;
	bool CollisionDetectImpl(StaticEntity* other, CollisionEvent** event) override;

	void HandleCollsion(PhysicalComponent* other, CollisionEvent* event)override;
	void HandleCollsionImpl(RigidBody* other, CollisionEvent* event) override;
	void HandleCollsionImpl(StaticEntity* other, CollisionEvent* event) override;
};

