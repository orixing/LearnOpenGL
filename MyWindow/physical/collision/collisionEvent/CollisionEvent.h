#pragma once

class PhysicalComponent;

//todo:�ػ�����
class CollisionEvent
{
public:
	PhysicalComponent* obj1;
	PhysicalComponent* obj2;

	CollisionEvent(PhysicalComponent* obj1, PhysicalComponent* obj2);

};

