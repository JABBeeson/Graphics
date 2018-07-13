#include "GameObject.h"
#include <iostream>

GameObject::GameObject()
{

}

void GameObject::Animate(int frame) {
	if (frame == 2) {
		//top left
		vertices.at(3) = 0.0f;
		vertices.at(4) = 0.5f;
		//top right
		vertices.at(8) = 1.0f;
		vertices.at(9) = 0.5f;
		//bottom left
		vertices.at(13) = 0.0f;
		vertices.at(14) = 0.0f;
		//bottom right
		vertices.at(18) = 1.0f;
		vertices.at(19) = 0.0f;

	}
	if (frame == 1) {
		//top left
		vertices.at(3) = 0.0f;
		vertices.at(4) = 1.0f;
		//top right
		vertices.at(8) = 1.0f;
		vertices.at(9) = 1.0f;
		//bottom left
		vertices.at(13) = 0.0f;
		vertices.at(14) = 0.5f;
		//bottom right
		vertices.at(18) = 1.0f;
		vertices.at(19) = 0.5f;

	}
	
	GenerateBuffers();
	
}

void GameObject::GenerateBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0); 
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shouldBuffer = false;
}

void GameObject::BulletGenerateBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shouldBuffer = false;
}

void GameObject::CoordVecCreation(GLfloat x, GLfloat y, GLfloat z, GLfloat texX, GLfloat texY){
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	vertices.push_back(texX);
	vertices.push_back(texY);
}

void GameObject::BulletCoordVecCreation(GLfloat x, GLfloat y, GLfloat z) {
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
}

void GameObject::Render(GLuint shaderProgram, glm::mat4 projectionMat, glm::mat4 viewMat) {

	if (shouldBuffer) GenerateBuffers(); // only if the vertex vertices need to change

	glUseProgram(shaderProgram);

	// bind texture here

	// transforms
	translation = glm::translate(projectionMat * viewMat, glm::vec3(x, y, z));
	rotation = glm::rotate(translation, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(zRot), glm::vec3(0.0f, 0.0f, 1.0f));
	scale = glm::scale(rotation, glm::vec3(xScl, yScl, zScl));
	//scale is the most up to date veriable

	GLint transLocation = glGetUniformLocation(shaderProgram, "trans");
	glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(scale));

	GLint uvoff = glGetUniformLocation(shaderProgram, "uvoff");
	glUniform2fv(uvoff, 1, glm::value_ptr(uvOffset));

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	// unbind texture 
}
void GameObject::BulletRender(GLuint shaderProgram, glm::mat4 projectionMat, glm::mat4 viewMat) {

	if (shouldBuffer) GenerateBuffers(); // only if the vertex vertices need to change

	glUseProgram(shaderProgram);

	// bind texture here

	// transforms
	translation = glm::translate(projectionMat * viewMat, glm::vec3(x, y, z));
	rotation = glm::rotate(translation, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(zRot), glm::vec3(0.0f, 0.0f, 1.0f));
	scale = glm::scale(rotation, glm::vec3(xScl, yScl, zScl));
	//scale is the most up to date veriable

	GLint transLocation = glGetUniformLocation(shaderProgram, "trans");
	glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(scale));

	//glBindTexture(GL_TEXTURE_2D, *texture);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	// unbind texture 
}

bool GameObject::CollisionCheck(GLfloat xOther, GLfloat yOther) {
	//std::cout << "collision check" << std::endl;
	if (xOther >= this->x - (this->width / 2) && xOther <= this->x + (this->width / 2)) {
		if (yOther >= this->y - (this->height / 2) && yOther <= this->y + (this->height / 2)) {
			return true;
		}
	}
	return false;
}

void GameObject::Move(GLfloat x, GLfloat y, GLfloat z) {
	this->x += x;
	this->y += y;
	this->z += z;
}
void GameObject::Rotate(GLfloat xRot, GLfloat yRot, GLfloat zRot) {
	this->xRot += xRot;
	this->yRot += yRot;
	this->zRot += zRot;
}
void GameObject::Scale(GLfloat xScl, GLfloat yScl, GLfloat zScl) {
	this->xScl += xScl;
	this->yScl += yScl;
	this->zScl += zScl;
}

GameObject::~GameObject()
{
}