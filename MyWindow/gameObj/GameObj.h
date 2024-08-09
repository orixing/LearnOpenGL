#pragma once
#include <glm/glm/glm.hpp>
#include "Model.h"
#include "Material.h"
class GameObj
{
public:
	glm::vec3 position = glm::vec3();
	float pitch; // ��X����ת
	float yaw;   // ��Y����ת
	float roll;  // ��Z����ת

	Model* model;

	GameObj(Model* model);
	virtual ~GameObj() = 0;

	virtual void Draw();
	virtual void DrawInGPass(Shader* shader);
	virtual void DrawInShadowPass(Shader* shader);

	//��ȡModel�任����
	glm::mat4 getModelMatrix();
};

