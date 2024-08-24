#include "RigidBody.h"
#include <algorithm>
#include <utility>
RigidBody::RigidBody(IPhysical* obj) : PhysicalComponent(obj){
	v = glm::vec3(0.0);
	w = glm::vec3(0.0);
	mass = 0.0f;
	float m = 1.0f;
	I = glm::mat4(0.0f);
	for (Vertex ver : obj->GetMesh()->vertices) {
		mass += m;
		glm::vec3 p = obj->GetScale() * ver.Position;
		I[0][0] += m * glm::dot(p, p);
		I[1][1] += m * glm::dot(p, p);
		I[2][2] += m * glm::dot(p, p);
		I[0][0] -= m * p.x * p.x;
		I[0][1] -= m * p.x * p.y;
		I[0][2] -= m * p.x * p.z;
		I[1][0] -= m * p.y * p.x;
		I[1][1] -= m * p.y * p.y;
		I[1][2] -= m * p.y * p.z;
		I[2][0] -= m * p.z * p.x;
		I[2][1] -= m * p.z * p.y;
		I[2][2] -= m * p.z * p.z;
	}
	I[3][3] = 1.0f;
}

void RigidBody::BeforeCollision() {
	v.y -= dt * g;
	v *= linear_decay;
	w *= angular_decay;
	totalJ = glm::vec3(0.0);
	totalT = glm::vec3(0.0);

	//每帧计算转动惯量的逆
	glm::mat4 rotationMatrix = glm::mat4_cast(obj->GetRotation());
	I_inverse = glm::inverse(rotationMatrix * I * glm::transpose(rotationMatrix));
}

void RigidBody::AfterCollision() {
	v += totalJ / mass;

	w += glm::vec3(I_inverse * glm::vec4(totalT, 0.0));

	obj->UpdatePosition(v * dt);
	obj->UpdateRotation(w * 0.5f * dt);

	std::cout << "V:		" << v.x << " " << v.y << " " << v.z << " " << std::endl;

}

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

void RigidBody::HandleCollision(IPhysical* otherObj) {
	//todo:目前只处理和地面的碰撞
	glm::vec3 P = glm::vec3(0, -0.75, 0);
	glm::vec3 N = glm::vec3(0, 1, 0);

	//算出所有平面下方点的平均点
	int pointNum = 0;
	glm::vec3 avgPointPos = glm::vec3(0.0);


	//边和面检测 todo:使用迭代器 

	vector<Vertex>* otherVertices = &otherObj->GetMesh()->vertices;
	vector<unsigned int>* otherIndices = &otherObj->GetMesh()->indices;

	vector<glm::vec3> collisionPoint;

	//边和每个面检测是否有穿过
	for (const std::pair<glm::vec3, glm::vec3>& line : obj->GetMesh()->lines) {
		for (int i = 0;i < otherIndices->size();i += 3) {
			glm::vec3 v0 = otherObj->getModelMatrix() * glm::vec4(otherVertices->at(otherObj->GetMesh()->indices[i]).Position, 1.0);
			glm::vec3 v1 = otherObj->getModelMatrix() * glm::vec4(otherVertices->at(otherObj->GetMesh()->indices[i+1]).Position, 1.0);
			glm::vec3 v2 = otherObj->getModelMatrix() * glm::vec4(otherVertices->at(otherObj->GetMesh()->indices[i+2]).Position, 1.0);
			glm::vec3 va = obj->getModelMatrix() * glm::vec4(line.first, 1.0);
			glm::vec3 vb = obj->getModelMatrix() * glm::vec4(line.second, 1.0);
			glm::vec3 x0a = va - v0;
			glm::vec3 xba = va - vb;
			glm::vec3 x10 = v0 - v1;
			glm::vec3 x20 = v0 - v2;
			float t = glm::dot(x0a, glm::cross(x10, x20)) / glm::dot(xba, glm::cross(x10, x20));
			if (t <= 1 && t >= 0) {
				glm::vec3 point = (1 - t) * va + t * vb;

				bool contain = false;
				for (glm::vec3 p : collisionPoint)
				{
					if (glm::length(p - point) < 0.01) {
						contain = true;
						break;
					}
				}
				if(!contain)
					collisionPoint.push_back(point);
				break;//这个边检测到碰撞了，下一个边
			}
		}
	}

	for (const glm::vec3& ver : collisionPoint) {
		glm::vec3 localPos = glm::inverse(obj->getModelMatrix()) * glm::vec4(ver, 1.0);
		//glm::vec3 radiusVec = obj->GetRadiusVector(ver.Position);
		glm::vec3 worldPos = obj->GetPosition() + obj->GetRotation() * localPos;
		//if (glm::dot(P - worldPos, N) < 0) continue;
		glm::vec3 pointTotalV = v + glm::cross(w, obj->GetRotation() * localPos);
		//如果是朝外运动，就不处理了
		if (glm::dot(pointTotalV, N) > 0) continue;
		//朝内运动，计数，记录位置
		pointNum++;
		avgPointPos += localPos;
	}

	if (pointNum == 0) return;
	avgPointPos = avgPointPos / (float)pointNum;
	//glm::vec3 R = obj->GetRadiusVector(avgPointPos);
	glm::vec3 avgPointTotalV = v + glm::cross(w, obj->GetRotation() * avgPointPos);//总速度
	glm::vec3 avgPointTotalVN = glm::dot(avgPointTotalV, N) * N;
	glm::vec3 avgPointTotalVT = avgPointTotalV - avgPointTotalVN;

	glm::vec3 newVN = glm::length(avgPointTotalVN) < 0.2 ? glm::vec3(0.0) : -restitution * avgPointTotalVN;
	glm::vec3 newVT = glm::length(avgPointTotalVT) < 0.001 ? glm::vec3(0.0)
		: avgPointTotalVT * std::max(0.0f, 1 - friction * (1 + restitution) * glm::length(avgPointTotalVN) / glm::length(avgPointTotalVT));
	glm::vec3 newV = newVN + newVT;

	if (glm::length(newV) < 0.1) 
		newV = glm::vec3(0.0);

	std::cout << "oldV:		" << avgPointTotalV.x << " " << avgPointTotalV.y << " " << avgPointTotalV.z << " " << std::endl;
	std::cout << "newV:		" << newV.x << " " << newV.y << " " << newV.z << " " << std::endl;

	//根据新旧速度算冲量
	glm::mat4 Rr_Matrix = VecToCrossMat(obj->GetRotation() * avgPointPos);
	glm::mat4 m1 = glm::mat4(0.0);
	m1[0][0] = m1[1][1] = m1[2][2] = m1[3][3] = 1.0f / mass;
	glm::mat4 K = m1 - Rr_Matrix * I_inverse * Rr_Matrix;
	glm::vec3 J = glm::inverse(K)* glm::vec4(newV - avgPointTotalV, 0.0);
	totalJ += J;//冲量
	totalT += glm::cross(obj->GetRotation() * avgPointPos, J);//力矩


	glm::vec3 testV = v + totalJ / mass;
	glm::vec3 testW = w + glm::vec3(I_inverse * glm::vec4(totalT, 0.0));

	glm::vec3 test = testV + glm::cross(testW, obj->GetRotation() * avgPointPos);//总速度

	std::cout << "test:		" << test.x << " " << test.y << " " << test.z << " " << std::endl;
}

