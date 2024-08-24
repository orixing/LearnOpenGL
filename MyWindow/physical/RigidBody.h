#pragma once
#include "Mesh.h";
#include "IPhysical.h";
#include "PhysicalComponent.h";
#include <glm/glm/glm.hpp>
class RigidBody : public PhysicalComponent
{
public:
	glm::vec3 v;//线速度
	glm::vec3 w;//角速度

	glm::vec3 totalJ;//碰撞产生的总冲量
	glm::vec3 totalT;//碰撞产生的总力矩

	float mass;
	glm::mat4 I;//todo:I受到缩放的影响，如果缩放改变需要重新计算，这里先假设缩放不变

	void BeforeCollision() override;
	void AfterCollision() override;
	void HandleCollision(IPhysical* otherObj) override;

	RigidBody(IPhysical* obj);

private:
	const float linear_decay = 0.998f;
	const float angular_decay = 0.98f;
	const float g = 9.8f;
	const float dt = 0.015f;
	const float restitution = 0.5f;
	const float friction = 0.4f;

	glm::mat4 I_inverse;//I施加旋转后得到全局转动惯量，再求逆，每次tick需要重新计算
};

