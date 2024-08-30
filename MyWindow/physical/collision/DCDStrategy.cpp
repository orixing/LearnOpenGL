#include "DCDStrategy.h"
#include "collision/collisionEvent/PointCollisionEvent.h"

bool DCDStrategy::MeshDCD(PhysicalComponent* one, PhysicalComponent* other, CollisionEvent** event) {
	//������н����ƽ����
	int pointNum = 0;
	glm::vec3 avgPointPos = glm::vec3(0.0);
	//�ߺ����� todo:ʹ�õ����� 
	vector<Vertex>* otherVertices = &other->obj->GetMesh()->vertices;
	vector<unsigned int>* otherIndices = &other->obj->GetMesh()->indices;

	
	static vector<glm::vec3> collisionPoint;
	collisionPoint.clear();

	//todo:�����Ż���cache������obj��
	glm::mat4 otherModel = other->obj->getModelMatrix();
	glm::mat4 objModel = one->obj->getModelMatrix();

	//�ߺ�ÿ�������Ƿ��д��� todo:�ⲿ��Ӧ�÷�����ײ�����
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
				break;//����߼�⵽��ײ�ˣ���һ����
			}
		}
	}

	//��Χ���ཻ��ʵ������ײ
	if (collisionPoint.size() == 0) {
		return false;
	}

	for (const glm::vec3& ver : collisionPoint) {
		pointNum++;
		avgPointPos += ver;
	}

	//����ײ����ƽ����ײ��

	*event = new PointCollisionEvent(one, other, avgPointPos / (float)pointNum);
	return true;

}