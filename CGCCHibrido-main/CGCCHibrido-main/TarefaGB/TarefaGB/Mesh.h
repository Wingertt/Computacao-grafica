#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Bezier.h"
#include <vector>


class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	void initialize(GLuint VAO, Shader* shader,
		float angle = 0.0, 
		glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0),
		float scale = 0.5f,
		float translateX = 0);
	void draw(GLuint texId);
	void update(glm::vec3 pointOnCurve);
	void setupTransformacoes(glm::mat4& model, glm::vec3 pointOnCurve, bool rotateX, bool rotateY, bool rotateZ, float scale);
	void setTexId(GLuint texId);
	GLuint getTexId();
	string getTexturePath();
	string getObjPath();
	string getMtlPath();
	void setDataVertices(vector<GLfloat> vertices);
	vector<GLfloat> getDataVertices();
	void setObjFilePath(string path);
	void setMtlFilePath(string path);
	void setTextureFilePath(string path);
	void setPosition(glm::vec3 position);
	void setRotateX();
	void setRotateY();
	void setRotateZ();
	void resetRotate();
	void incrementScale(float scaleFacor);
	void decrementScale(float scaleFactor);
	void setBasePoints(std::vector<glm::vec3> basePoints);
	glm::vec3 getPosition();
	void decrementTranslateX(float translateStep);
	void incrementTranslateX(float translateStep);

protected:
	GLuint VAO; //Identificador do Vertex Array Object - V�rtices e seus atributos
	int nVertices;

	//Informa��es sobre as transforma��es a serem aplicadas no objeto
	glm::vec3 position;
	glm::vec3 scale;
	float angle;
	glm::vec3 axis;

	//Refer�ncia (endere�o) do shader
	Shader* shader;
	Bezier bezier;
	
	GLuint texId;
	string textFilePath;
	string objFilePath;
	string mtlFilePath;
	
	bool rotateX = false;
	bool rotateY = false;
	bool rotateZ = false;
	float scaleLevel;
	bool isStatic;
	float translateX;

	vector<GLfloat> dataVertices;

	std::vector<glm::vec3> basePoints;
};

