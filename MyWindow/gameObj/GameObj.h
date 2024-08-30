#pragma once
#include <glm/glm/glm.hpp>
#include "Model.h"
#include "Material.h"
#include "IPhysical.h"
#include "PhysicalComponent.h"

class GameObj : public IPhysical
{
public:
	glm::vec3 position = glm::vec3();
	float pitch; // ��X����ת
	float yaw;   // ��Y����ת
	float roll;  // ��Z����ת

	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float scaleZ = 1.0f;

	Model* model;

	GameObj(Model* model);
	virtual ~GameObj() = 0;

	virtual void Draw();
	virtual void DrawInGPass(Shader* shader);
	virtual void DrawInShadowPass(Shader* shader);

	//��ȡModel�任����
	glm::mat4 getModelMatrix() override;
	glm::quat getRotationQuad();
	void setRotationByQuad(glm::quat q);

	bool renderBorder = false;
	bool transparent = false;

	long guid;

	Mesh* GetMesh() override;
	void UpdatePosition(glm::vec3 deltaX) override;
	void UpdateRotation(glm::vec3 dw) override;
	glm::vec3 GetPosition() override;
	glm::quat GetRotation() override;
	glm::vec3 GetScale() override;
	glm::vec3 GetRadiusVector(glm::vec3 localPos) override;
	long GetGuid() override;

	PhysicalComponent* physical;
};

