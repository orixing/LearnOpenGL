#include "PhysicalComponent.h"

void PhysicalComponent::BeforeCollision() {}
void PhysicalComponent::AfterCollision() {}
void PhysicalComponent::HandleCollision(IPhysical* otherObj) {}

PhysicalComponent::PhysicalComponent(IPhysical* obj) :obj(obj) {}
