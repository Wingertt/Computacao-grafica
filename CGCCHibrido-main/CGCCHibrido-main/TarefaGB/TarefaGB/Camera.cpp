#pragma once

#include "Camera.h"

void Camera::initialize(Shader* shader, int width, int height, float minFrustum, float maxFrustum, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, bool firstMouse,
	float lastX, float lastY, float sensitivity, float pitch, float yaw)
{
	this->shader = shader;

	this->cameraPos = cameraPos;
	this->cameraFront = cameraFront;
	this->cameraUp = cameraUp;

	this->firstMouse = firstMouse;
	this->lastX = lastX;
	this->lastY = lastY;
	this->sensitivity = sensitivity;
	this->pitch = pitch;
	this->yaw = yaw;

	//Matriz de view -- posi??o e orienta??o da c?mera
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader->setMat4("view", value_ptr(view));

	//Matriz de proje??o perspectiva - definindo o volume de visualiza??o (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, minFrustum, maxFrustum);
	shader->setMat4("projection", glm::value_ptr(projection));
}

void Camera::update()
{
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	shader->setMat4("view", glm::value_ptr(view));

	//Atualizando o shader com a posicao da camera
	shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
}

void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float offsetx = xpos - lastX;
	float offsety = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	offsetx *= sensitivity;
	offsety *= sensitivity;

	pitch += offsety;
	yaw += offsetx;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}

void Camera::setCameraPos(int key, float cameraSpeed) {
	if (key == GLFW_KEY_W)
	{
		cameraPos += (cameraFront * cameraSpeed);
	}
	else if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraFront * cameraSpeed;
	}
	else if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	else if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}