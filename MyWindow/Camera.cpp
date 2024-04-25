#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 direc) {
	Position = position;
	Direc = direc;
	updateAttribute();
}

void Camera::ProcessKeyboard(DirecEnum direction, float deltaTime) {
	float deltaValue = speed * deltaTime;
	switch (direction)
	{
	case DirecEnum::Front:
		//Position += glm::vec3(Direc.x, 0, Direc.z) * deltaValue;
		Position += Direc * deltaValue;
		break;
	case DirecEnum::Back:
		//Position -= glm::vec3(Direc.x, 0, Direc.z) * deltaValue;
		Position -= Direc * deltaValue;
		break;
	case DirecEnum::Left:
		Position -= Right * deltaValue;
		break;
	case DirecEnum::Right:
		Position += Right * deltaValue;
		break;
	default:
		break;
	}
}
void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
	xoffset *= sensitive;
	yoffset *= sensitive;

	Yaw += xoffset;
	Pitch += yoffset;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	updateAttribute();
}
void Camera::ProcessMouseScroll(float yoffset) {
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void Camera::updateAttribute() {
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Direc = glm::normalize(front);

	//std::cout << Direc.x<< Direc.y<< Direc.z << std::endl;

	Right = glm::normalize(glm::cross(Direc, glm::vec3(0.0f,1.0f,0.0f))); 
	Up = glm::normalize(glm::cross(Right, Direc));
	std::cout << Right.x << Right.y << Right.z << std::endl;
	std::cout << Up.x << Up.y << Up.z << std::endl;
}

glm::mat4  Camera::GetViewMatrix() {
	return glm::lookAt(Position, Position + Direc, Up);
}
