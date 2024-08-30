#include "DCDStrategy.h"
#include "collision/collisionEvent/PointCollisionEvent.h"

bool DCDStrategy::MeshDCD(PhysicalComponent* one, PhysicalComponent* other, CollisionEvent** event) {
	//算出所有交点的平均点
	int pointNum = 0;
	glm::vec3 avgPointPos = glm::vec3(0.0);
	//边和面检测 todo:使用迭代器 
	vector<Vertex>* otherVertices = &other->obj->GetMesh()->vertices;
	vector<unsigned int>* otherIndices = &other->obj->GetMesh()->indices;

	
	static vector<glm::vec3> collisionPoint;
	collisionPoint.clear();

	//todo:换种优化，cache保存在obj里
	glm::mat4 otherModel = other->obj->getModelMatrix();
	glm::mat4 objModel = one->obj->getModelMatrix();

	//边和每个面检测是否有穿过 todo:这部分应该放在碰撞检测里
	for (const std::pair<glm::vec3, glm::vec3>& line : one->obj->GetMesh()->lines) {
		for (int i = 0; i < otherIndices->size(); i += 3) {
			glm::vec3 v0 = otherModel * glm::vec4(otherVertices->at(other->obj->GetMesh()->indices[i]).Position, 1.0);
			glm::vec3 v1 = otherModel * glm::vec4(otherVertices->at(other->obj->GetMesh()->indices[i + 1]).Position, 1.0);
			glm::vec3 v2 = otherModel * glm::vec4(otherVertices->at(other->obj->GetMesh()->indices[i + 2]).Position, 1.0);
			glm::vec3 va = objModel * glm::vec4(line.first, 1.0);
			glm::vec3 vb = objModel * glm::vec4(line.second, 1.0);
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
				if (!contain)
					collisionPoint.push_back(point);
				break;//这个边检测到碰撞了，下一个边
			}
		}
	}

	//包围盒相交但实际无碰撞
	if (collisionPoint.size() == 0) {
		return false;
	}

	for (const glm::vec3& ver : collisionPoint) {
		pointNum++;
		avgPointPos += ver;
	}

	//有碰撞，算平均碰撞点

	*event = new PointCollisionEvent(one, other, avgPointPos / (float)pointNum);
	return true;

}