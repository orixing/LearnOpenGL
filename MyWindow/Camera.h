#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

enum class DirecEnum {
	Front, Back, Left, Right
};

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Direc;
	glm::vec3 Up;
	glm::vec3 Right;

	float Yaw = -90.0f;
	float Pitch = 0.0f;
	float speed = 2.5f;
	float sensitive = 0.4f;
	float fov = 45.0f;

	Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,4.0f));
	void ProcessKeyboard(DirecEnum direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessMouseScroll(float yoffset);
	glm::mat4 GetViewMatrix();

private:
	void updateAttribute();
};

#endif CAMERA_H
