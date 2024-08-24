#include "PhysicalComponent.h"

void PhysicalComponent::BeforeCollision() {}
void PhysicalComponent::AfterCollision() {}
void PhysicalComponent::HandleCollision(IPhysical* otherObj) {}

PhysicalComponent::PhysicalComponent(IPhysical* obj) :obj(obj) {}


void PhysicalComponent::BuildBoundingBox() {
	obj->boundingBox->Clear();
	for (const Vertex& ver : obj->GetMesh()->vertices) {
		glm::vec3 worldPos = obj->getModelMatrix() * glm::vec4(ver.Position,1.0);
		obj->boundingBox->AddOneVertex(worldPos);
	}
}