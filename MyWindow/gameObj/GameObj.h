#pragma once
#include <glm/glm/glm.hpp>
#include "Model.h"
#include "Material.h"
class GameObj
{
public:
	glm::vec3 position = glm::vec3();
	float pitch; // 绕X轴旋转
	float yaw;   // 绕Y轴旋转
	float roll;  // 绕Z轴旋转

	Model* model;

	GameObj(Model* model);
	virtual ~GameObj() = 0;

	virtual void Draw();
	virtual void DrawInGPass(Shader* shader);
	virtual void DrawInShadowPass(Shader* shader);

	//获取Model变换矩阵
	glm::mat4 getModelMatrix();
};

