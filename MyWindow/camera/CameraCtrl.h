#pragma once
#include "Camera.h";
class CameraCtrl
{
public:
	static CameraCtrl& getInstance();

	CameraCtrl(const CameraCtrl& cameraCtrl) = delete;
	CameraCtrl& operator=(const CameraCtrl& cameraCtrl) = delete;

	Camera* mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 4.0f));

private:
	CameraCtrl();
	~CameraCtrl();
};

