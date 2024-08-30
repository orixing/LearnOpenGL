#pragma once
#include "Mesh.h"
#include "IPhysical.h"
#include "BoundingBox.h"
#include "collision/collisionEvent/CollisionEvent.h"
class RigidBody;
class StaticEntity;

class PhysicalComponent 
{
public:
	virtual void BuildBoundingBox();
	virtual void BeforeCollision();
	virtual void AfterCollision();
	//virtual void HandleCollision(IPhysical* otherObj);
	IPhysical* obj;
	PhysicalComponent(IPhysical* obj);
	void SetDynamic(bool b);
	bool IsDynamic() { return dynamic; }
	virtual void OnDynamic();
	virtual void OnStable();

	BoundingBox* boundingBox;//todo:��Χ��Ӧ�÷���Obj����

	//˫������ײ���
	virtual bool DiscreteCollisionDetect(PhysicalComponent* other, CollisionEvent** event) = 0;
	virtual bool CollisionDetectImpl(RigidBody* other, CollisionEvent** event) = 0;
	virtual bool CollisionDetectImpl(StaticEntity* other, CollisionEvent** event) = 0;

	//˫������ײ����
	virtual void HandleCollsion(PhysicalComponent* other, CollisionEvent* event) = 0;
	virtual void HandleCollsionImpl(RigidBody* other, CollisionEvent* event) = 0;
	virtual void HandleCollsionImpl(StaticEntity* other, CollisionEvent* event) = 0;

private:
	bool dynamic = false;
};

