﻿#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>

using namespace std;
using std::string;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Bezier.h"

//window configuration
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void setupWindow(GLFWwindow*& window);
void setupShader(Shader shader);

//geometry configuration
void readFromObj(Mesh& object);
void readFromMtl(Mesh& object);
int setupGeometry(std::vector<GLfloat> vertices);
void loadTexture(Mesh& object);
Mesh loadObjectConfig(string path);

// Parametric curves
std::vector<glm::vec3> generateTranslatePoints(string path);

//file values
string basePath = "../../3D_Models/";
void loadCameraConfig(string path, int width, int height, Shader& shader);

//iluminacao values
std::vector<GLfloat> ka;
std::vector<GLfloat> ks;
float ns;

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05f;
float pitch = 0.0, yaw = -90.0;

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

Camera camera;

std::vector<string> objectsConfig;
std::vector<Mesh> objects;
std::vector<GLuint> vaos;

int keyIndex;

int main() {
	GLFWwindow* window;

	setupWindow(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");

	// Definição dos objetos a serem usados em cena
	// Cada objeto deve ter seu próprio arquivo txt de configuração dentro da pasta "Arquivos/config"
	objectsConfig.push_back("cubo.txt");
	objectsConfig.push_back("bola.txt");
	objectsConfig.push_back("suzanne.txt");
	objectsConfig.push_back("planeta.txt");

	// Inicialização dos objetos em cena, leitura dos dados dos arquivos de configuração .txt .obj .mtl e texturas
	for (const string& config : objectsConfig) {
		std::cout << "Object config: " << config << std::endl;
		Mesh object = loadObjectConfig(config);

		readFromObj(object);
		readFromMtl(object);
		loadTexture(object);

		GLuint VAO = setupGeometry(object.getDataVertices());
		vaos.push_back(VAO);

		object.initialize(VAO, &shader);

		objects.push_back(object);
	}

	glUseProgram(shader.ID);
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	setupShader(shader);

	glEnable(GL_DEPTH_TEST);

	loadCameraConfig("camera.txt", width, height, shader);

	std::vector<glm::vec3> basePoints = generateTranslatePoints(basePath + "Cube/circle.txt");

	Bezier bezier;
	bezier.setControlPoints(basePoints);
	bezier.setShader(&shader);
	bezier.generateCurve(30);
	int nbCurvePoints = bezier.getNbCurvePoints();

	int i = 0;
	int speed = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		camera.update();

		glm::vec3 pointOnCurve1 = bezier.getPointOnCurve(i);

		objects[0].update(pointOnCurve1);
		objects[0].draw(objects[0].getTexId());

		//Inicialização dos objetos em cena
		int j = 0;
		for (Mesh& object: objects) {
			if (j++ > 0) {
				object.update(object.getPosition());
				object.draw(object.getTexId());
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		if (speed++ == 50) {
			speed = 0;
			i = (i + 1) % nbCurvePoints;
		}

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vaos[0]);
	glDeleteVertexArrays(1, &vaos[1]);
	glDeleteVertexArrays(1, &vaos[2]);
	glfwTerminate();
	return 0;
}

Mesh loadObjectConfig(string path) {
	Mesh object;

	std::ifstream file(basePath + "/config/" + path);

	if (!file.is_open())
		std::cout << "Failed to open the config file. = config/" + path << std::endl;

	string line;
	string objFilePath;
	string textureFilePath;
	string name;
	string mtlFilePath;
	glm::vec3 position;
	std::vector<glm::vec3> temp_basePoints;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);
			string prefix;
			iss >> prefix;

			if (prefix == "name") {
				iss >> name;
				std::cout << name << std::endl;
			}
			else if (prefix == "obj") {
				iss >> objFilePath;
				std::cout << objFilePath << std::endl;
			}
			else if (prefix == "position") {
				char comma;
				iss >> position.x >> comma >> position.y >> comma >> position.z;
			}
			else if (prefix == "t") {
				char comma;
				iss >> position.x >> comma >> position.y >> comma >> position.z;

				temp_basePoints.push_back(position);
			}
			else if (prefix == "initX")
				object.setRotateX();
			else if (prefix == "initY")
				object.setRotateY();
			else if (prefix == "initZ")
				object.setRotateZ();
		}
	}

	file.close();

	object.setObjFilePath(objFilePath);
	object.setPosition(position);
	object.setBasePoints(temp_basePoints);

	return object;
}

void loadCameraConfig(string path, int width, int height, Shader& shader){
	Mesh object;
	glm::vec3 pos;
	std::ifstream file(basePath + "/config/" + path);

	if (!file.is_open())
		std::cout << "Failed to open the config file. = config/" + path << std::endl;

	string line;
	float minFrustum;
	float maxFrustum;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);
			string prefix;
			iss >> prefix;
			if (prefix == "pos") {
				char comma;
				iss >> pos.x >> comma >> pos.y >> comma >> pos.z;
			}
			else if (prefix == "minFrustum") {
				iss >> minFrustum;
			}
			else if (prefix == "maxFrustum") {
				iss >> maxFrustum;
			}
		}
	}

	file.close();

	camera.initialize(&shader, width, height, minFrustum, maxFrustum , pos);
}

void setupShader(Shader shader) {
	shader.setVec3("ka", ka[0], ka[1], ka[2]);
	shader.setFloat("kd", 0.7f);
	shader.setVec3("ks", ks[0], ks[1], ks[2]);
	shader.setFloat("q", ns);

	shader.setVec3("lightPos", -2.0f, 100.0f, 2.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
}

void readFromMtl(Mesh& object) {
	string path = basePath + object.getMtlPath();
	std::ifstream file(path);

	cout << path << endl;

	if (!file.is_open()) 
		std::cout << "Failed to open mtl file." + path << std::endl;

	string line, textureFilePath;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);
			string prefix;
			iss >> prefix;

			if (prefix == "map_Kd") {
				iss >> textureFilePath;
				object.setTextureFilePath(textureFilePath);
			}
			else if (prefix == "Ka") {
				glm::vec3 temp_ka;
				iss >> temp_ka.x >> temp_ka.y >> temp_ka.z;
				iss >> temp_ka.x >> temp_ka.y >> temp_ka.z;

				ka.push_back(temp_ka.x);
				ka.push_back(temp_ka.y);
				ka.push_back(temp_ka.z);
			}
			else if (prefix == "Ks") {
				glm::vec3 temp_ks;
				iss >> temp_ks.x >> temp_ks.y >> temp_ks.z;

				ks.push_back(temp_ks.x);
				ks.push_back(temp_ks.x);
				ks.push_back(temp_ks.x);
			}
			else if (prefix == "Ns") 
				iss >> ns;
		}
	}

	file.close();
}

int setupGeometry(std::vector<GLfloat> vertices) {
	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	// coordenadas posição - x, y, z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	// coordenadas de textura - s, t (ou u, v)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// coordenadas normal - x, y, z
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

void readFromObj(Mesh& object) {
	string path = basePath + object.getObjPath();

	std::ifstream file(path);

	if (!file.is_open())
		std::cout << "Failed to open the file." + path << std::endl;

	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_textures;
	std::vector<glm::vec3> temp_normais;
	std::vector<GLfloat> temp_total;

	string line;
	string mtlFilePath;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);

			string prefix;
			iss >> prefix;

			if (prefix == "v") {

				glm::vec3 values;
				iss >> values.x >> values.y >> values.z;
				temp_vertices.push_back(values);
			}
			else if (prefix == "vt") {
				glm::vec2 values;
				iss >> values.x >> values.y;
				temp_textures.push_back(values);
			}
			else if (prefix == "vn") {
				glm::vec3 values;
				iss >> values.x >> values.y >> values.z;
				temp_normais.push_back(values);
			}
			else if (prefix == "f") {
				unsigned int vertexIndex, textIndex, normalIndex;
				char slash;

				for (int i = 0; i < 3; ++i) {
					iss >> vertexIndex >> slash >> textIndex >> slash >> normalIndex;

					glm::vec3 verticess = temp_vertices[vertexIndex - 1];
					glm::vec3 normaiss = temp_normais[normalIndex - 1];
					glm::vec2 texturess = temp_textures[textIndex - 1];

					temp_total.push_back(verticess.x);
					temp_total.push_back(verticess.y);
					temp_total.push_back(verticess.z);

					temp_total.push_back(texturess.x);
					temp_total.push_back(texturess.y);

					temp_total.push_back(normaiss.x);
					temp_total.push_back(normaiss.y);
					temp_total.push_back(normaiss.z);
				}
			}
			else if (prefix == "mtllib") {
				iss >> mtlFilePath;
				object.setMtlFilePath(mtlFilePath);
			}
		}
	}

	file.close();

	object.setDataVertices(temp_total);
}

void loadTexture(Mesh& object) {
	string path = basePath + object.getTexturePath();
	GLuint texID;

	cout << path << endl;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data) {
		if (nrChannels == 3) 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		else 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		cout << "Failed to load texture" + path << endl;

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	object.setTexId(texID);
}

void setupWindow(GLFWwindow*& window) {
	glfwInit();

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tarefa GB", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	//Desabilita o desenho do cursor 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
		std::cout << "Failed to initialize GLAD" << std::endl;

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.mouseCallback(window, xpos, ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	float scaleIncrement = 0.1f;
	float translateStep = 0.1f;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= GLFW_KEY_0 && key <= GLFW_KEY_3) {
		keyIndex = key - GLFW_KEY_0;
		std::cout << keyIndex << std::endl;
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS) 
		objects[keyIndex].setRotateX();

	if (key == GLFW_KEY_Y && action == GLFW_PRESS) 
		objects[keyIndex].setRotateY();

	if (key == GLFW_KEY_Z && action == GLFW_PRESS) 
		objects[keyIndex].setRotateZ();

	if (key == GLFW_KEY_P && action == GLFW_PRESS) 
		objects[keyIndex].resetRotate();

	if (key == GLFW_KEY_R && action == GLFW_PRESS) 
		objects[keyIndex].incrementScale(scaleIncrement);

	if (key == GLFW_KEY_T && action == GLFW_PRESS) 
		objects[keyIndex].decrementScale(scaleIncrement);

	// Translação -----------------
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		switch (key) {
			case GLFW_KEY_I:
				objects[keyIndex].incrementTranslateX(translateStep);
				break;
			case GLFW_KEY_J:
				objects[keyIndex].decrementTranslateX(translateStep);
				break;
			default:
				break;
		}

	camera.setCameraPos(key);
}

std::vector<glm::vec3> generateTranslatePoints(string path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "Failed to open the points file." + path << std::endl;
	}

	string line;
	vector <glm::vec3> points;

	while (std::getline(file, line)) {
		if (line.length() > 0) {

			std::istringstream iss(line);
			string prefix;
			char comma;

			glm::vec3 temp_points;
			iss >> temp_points.x >> comma >> temp_points.y >> comma >> temp_points.z;

			points.push_back(temp_points);
		}
	}

	file.close();
	return points;
}
