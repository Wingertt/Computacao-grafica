#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"


class Camera
{
public:
	Camera() {}
	~Camera() {}
	void initialize(
		Shader* shader,
		int width, int height
	);
	void update();
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void setCameraPos(int key, float cameraSpeed = 0.5f);

protected:

	float sensitivity;
	float pitch;
	float yaw;
	float lastX;
	float lastY;
	bool firstMouse;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 front;

	Shader* shader;
};
