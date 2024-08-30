#pragma once
#include "Mesh.h";
#include "IPhysical.h";
#include "PhysicalComponent.h";
#include <glm/glm/glm.hpp>
class RigidBody : public PhysicalComponent
{
public:
	glm::vec3 v;//���ٶ�
	glm::vec3 w;//���ٶ�

	glm::vec3 totalJ;//��ײ�������ܳ���
	glm::vec3 totalT;//��ײ������������

	float mass;
	glm::mat4 I;//todo:I�ܵ����ŵ�Ӱ�죬������Ÿı���Ҫ���¼��㣬�����ȼ������Ų���

	void BeforeCollision() override;
	void AfterCollision() override;
	//void HandleCollision(IPhysical* otherObj) override;
	void OnDynamic() override;
	void OnStable() override;

	RigidBody(IPhysical* obj);

	bool DiscreteCollisionDetect(PhysicalComponent* other, CollisionEvent** event) override;
	bool CollisionDetectImpl(RigidBody* other, CollisionEvent** event) override;
	bool CollisionDetectImpl(StaticEntity* other, CollisionEvent** event) override;

	void HandleCollsion(PhysicalComponent* other, CollisionEvent* event)override;
	void HandleCollsionImpl(RigidBody* other, CollisionEvent* event) override;
	void HandleCollsionImpl(StaticEntity* other, CollisionEvent* event) override;

	glm::mat4 I_inverse;//Iʩ����ת��õ�ȫ��ת�������������棬ÿ��tick��Ҫ���¼���
	const float restitution = 0.5f;
	const float friction = 0.4f;

private:
	const float linear_decay = 0.998f;
	const float angular_decay = 0.98f;
	const float g = 9.8f;
	const float dt = 0.015f;
};

