#pragma once

#include "Camera.h"

void Camera::initialize(Shader* shader, int width, int height)
{
	this->shader = shader;

	cameraPos = glm::vec3(0.0, 0.0, 3.0);
	cameraFront = glm::vec3(0.0, 0.0, 0.0);
	cameraUp = glm::vec3(0.0, 1.0, 0.0);
	firstMouse = true;
	lastX = 0;
	lastY = 0;
	sensitivity = 0.05f;
	pitch = 0.0;
	yaw = -90.0;

	//posicao e orientacao
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader->setMat4("view", value_ptr(view));

	//definindo o volume de visualizacao (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader->setMat4("projection", glm::value_ptr(projection));
}

void Camera::update()
{
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	shader->setMat4("view", glm::value_ptr(view));

	//Atualizando o shader com a posicao da camera
	shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
}

void Camera::setCameraPos(int key, float cameraSpeed) {

	if (key == GLFW_KEY_W)
		cameraPos += (cameraFront * cameraSpeed);

	if (key == GLFW_KEY_S)
		cameraPos -= cameraFront * cameraSpeed;

	if (key == GLFW_KEY_A)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (key == GLFW_KEY_D)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	//garante funcionamento inicial adequado da camera em relacao a origem do objeto
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	//movimento eixo X e eixo Y
	pitch -= (ypos - lastY) * sensitivity;
	yaw   += (xpos - lastX) * sensitivity;

	lastX = xpos;
	lastY = ypos;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}
