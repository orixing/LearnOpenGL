#pragma once
#include "Mesh.h"
#include "IPhysical.h"
class PhysicalComponent 
{
public:
	virtual void BuildBoundingBox();
	virtual void BeforeCollision();
	virtual void AfterCollision();
	virtual void HandleCollision(IPhysical* otherObj);
	IPhysical* obj;
	PhysicalComponent(IPhysical* obj);
	void SetDynamic(bool b);
	bool IsDynamic() { return dynamic; }
	virtual void OnDynamic();
	virtual void OnStable();
private:
	bool dynamic = false;
};

