#include "CollisionHandlerStrategy.h"
#include "collision/collisionEvent/PointCollisionEvent.h"
glm::mat4 VecToCrossMat(glm::vec3 v)
{
	glm::mat4 ret = glm::mat4(0.0);
	ret[0][1] = -v[2];
	ret[0][2] = v[1];
	ret[1][0] = v[2];
	ret[1][2] = -v[0];
	ret[2][0] = -v[1];
	ret[2][1] = v[0];
	ret[3][3] = 1;
	return ret;
}

void CollisionHandlerStrategy::RigidStaticCollision(RigidBody* rigid, StaticEntity* staticEntity, CollisionEvent* event) {

	//todo:目前只处理和地面的碰撞
	glm::vec3 P = glm::vec3(0, -0.75, 0);
	glm::vec3 N = glm::vec3(0, 1, 0);

	//todo:换种优化，cache保存在obj里
	glm::mat4 objModel = rigid->obj->getModelMatrix();

	glm::vec3 avgPointPos = static_cast<PointCollisionEvent*>(event)->collisionPoint;

	glm::vec3 localPos = glm::inverse(objModel) * glm::vec4(avgPointPos, 1.0);

	glm::vec3 avgPointTotalV = rigid->v + glm::cross(rigid->w, rigid->obj->GetRotation() * localPos);
	//如果是朝外运动，就不处理了
	if (glm::dot(avgPointTotalV, N) > 0) return;

	glm::vec3 avgPointTotalVN = glm::dot(avgPointTotalV, N) * N;
	glm::vec3 avgPointTotalVT = avgPointTotalV - avgPointTotalVN;

	glm::vec3 newVN = glm::length(avgPointTotalVN) < 0.1 ? glm::vec3(0.0) : -rigid->restitution * avgPointTotalVN;
	glm::vec3 newVT = glm::length(avgPointTotalVT) < 0.1 ? glm::vec3(0.0)
		: avgPointTotalVT * std::max(0.0f, 1 - rigid->friction * (1 + rigid->restitution) * std::max(glm::length(avgPointTotalVN), 0.1f) / glm::length(avgPointTotalVT));
	glm::vec3 newV = newVN + newVT;

	//if (glm::length(newV) < 0.1) 
	//	newV = glm::vec3(0.0);

	//std::cout << "oldV:        " << avgPointTotalV.x << " " << avgPointTotalV.y << " " << avgPointTotalV.z << " " << std::endl;
	//std::cout << "newV:        " << newV.x << " " << newV.y << " " << newV.z << " " << std::endl;

	//根据新旧速度算冲量
	glm::mat4 Rr_Matrix = VecToCrossMat(rigid->obj->GetRotation() * localPos);
	glm::mat4 m1 = glm::mat4(0.0);
	m1[0][0] = m1[1][1] = m1[2][2] = m1[3][3] = 1.0f / rigid->mass;
	glm::mat4 K = m1 - Rr_Matrix * rigid->I_inverse * Rr_Matrix;
	glm::vec3 J = glm::inverse(K) * glm::vec4(newV - avgPointTotalV, 0.0);
	rigid->totalJ += J;//冲量
	rigid->totalT += glm::cross(rigid->obj->GetRotation() * localPos, J);//力矩

	glm::vec3 testV = rigid->v + rigid->totalJ / rigid->mass;
	glm::vec3 testW = rigid->w + glm::vec3(rigid->I_inverse * glm::vec4(rigid->totalT, 0.0));
	glm::vec3 test = testV + glm::cross(testW, rigid->obj->GetRotation() * localPos);
	//std::cout << "test:        " << test.x << " " << test.y << " " << test.z << " " << std::endl;
}