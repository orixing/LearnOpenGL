#include "StaticEntity.h"
#include "collision/DCDStrategy.h"
#include "RigidBody.h"
#include "collision/CollisionHandlerStrategy.h"

StaticEntity::StaticEntity(IPhysical* obj) : PhysicalComponent(obj) {
}

bool StaticEntity::DiscreteCollisionDetect(PhysicalComponent* other, CollisionEvent** event) {
	return other->CollisionDetectImpl(this, event);
}
//todo:现在的MeshDCD是临时的，第一个参数是物体，第二个参数是地面，一般化后参数顺序可以改回来 
bool StaticEntity::CollisionDetectImpl(RigidBody* other, CollisionEvent** event) {
	return DCDStrategy::MeshDCD(other, this, event);
}
bool StaticEntity::CollisionDetectImpl(StaticEntity* other, CollisionEvent** event) {
	return DCDStrategy::MeshDCD(other, this, event);
}



void StaticEntity::HandleCollsion(PhysicalComponent* other, CollisionEvent* event) {
	other->HandleCollsionImpl(this, event);
}
void StaticEntity::HandleCollsionImpl(RigidBody* other, CollisionEvent* event) {
	CollisionHandlerStrategy::RigidStaticCollision(other, this, event);
}
void StaticEntity::HandleCollsionImpl(StaticEntity* other, CollisionEvent* event) {
	//静态物体之间不处理
	return;
}

