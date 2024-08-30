#include "StaticEntity.h"
#include "collision/DCDStrategy.h"
#include "RigidBody.h"
#include "collision/CollisionHandlerStrategy.h"

StaticEntity::StaticEntity(IPhysical* obj) : PhysicalComponent(obj) {
}

bool StaticEntity::DiscreteCollisionDetect(PhysicalComponent* other, CollisionEvent** event) {
	return other->CollisionDetectImpl(this, event);
}
//todo:���ڵ�MeshDCD����ʱ�ģ���һ�����������壬�ڶ��������ǵ��棬һ�㻯�����˳����ԸĻ��� 
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
	//��̬����֮�䲻����
	return;
}

