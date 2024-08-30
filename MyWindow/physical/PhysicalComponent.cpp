#include "PhysicalComponent.h"

void PhysicalComponent::BeforeCollision() {}
void PhysicalComponent::AfterCollision() {}
//void PhysicalComponent::HandleCollision(IPhysical* otherObj) {}

PhysicalComponent::PhysicalComponent(IPhysical* obj) :obj(obj) {
	boundingBox = new BoundingBox();
	BuildBoundingBox();
}


void PhysicalComponent::BuildBoundingBox() {
	boundingBox->Clear();
	glm::mat4 model = obj->getModelMatrix();
	for (const Vertex& ver : obj->GetMesh()->vertices) {
		glm::vec3 worldPos = model * glm::vec4(ver.Position,1.0);
		boundingBox->AddOneVertex(worldPos);
	}
}

void PhysicalComponent::SetDynamic(bool b) {
	if (!dynamic && b) OnDynamic();
	if (dynamic && !b) OnStable();
	dynamic = b;
}
void PhysicalComponent::OnDynamic(){}
void PhysicalComponent::OnStable() {}