#include "PhysicalComponent.h"

void PhysicalComponent::BeforeCollision() {}
void PhysicalComponent::AfterCollision() {}
void PhysicalComponent::HandleCollision(IPhysical* otherObj) {}

PhysicalComponent::PhysicalComponent(IPhysical* obj) :obj(obj) {}


void PhysicalComponent::BuildBoundingBox() {
	obj->boundingBox->Clear();
	glm::mat4 model = obj->getModelMatrix();
	for (const Vertex& ver : obj->GetMesh()->vertices) {
		glm::vec3 worldPos = model * glm::vec4(ver.Position,1.0);
		obj->boundingBox->AddOneVertex(worldPos);
	}
}

void PhysicalComponent::SetDynamic(bool b) {
	dynamic = b;
	if (b) OnDynamic();
	if (!b) OnStable();
}
void PhysicalComponent::OnDynamic(){}
void PhysicalComponent::OnStable() {}