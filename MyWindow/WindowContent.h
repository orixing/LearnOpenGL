#pragma once
#include "Camera.h"
#include <vector>
#include "GameObj.h"
#include "Light.h"
#include "SkyboxObj.h"
//һ��Window������������
class WindowContent {
public:
	Camera* mainCamera;
	bool useFXAA;
	float mousePosX;
	float mousePosY;
	bool firstMouse;

	float curFrameTime;
	float lastFrameTime;
	float frameDeltaTime;

	std::vector<GameObj*> *allObjs;
	std::vector<Light*>* allLights;

	SkyboxObj* skyboxObj;

	FrameBuffer* GBuffer, * SSAOFBO, * SSAOBlurFBO, * postProcessingFBO;

	WindowContent(Camera* c);
	~WindowContent();

	void ShootNewObj();//todo:ʹ�ù���ʵ�֣�������Ҫ�����ط�

	Model* model;//todo:һ��modelֻ����һ�Σ�����Obj�Ĺ���

};