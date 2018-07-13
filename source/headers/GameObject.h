#pragma once
#include <vector>
// GLEW - OpenGL Extension Wrangler 
#include <GL/glew.h>

// SDL - Simple DirectMedia Layer 
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#define GLM_FORCE_RADIANS // force glm to use radians
// NOTE: must do before including GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GameObject
{
public:
	int frame = 1;
	GLfloat width, height;
	GLfloat x = 0.0f, y = 0.0f, z = 0.0f; //positional data
	GLfloat xRot = 0.0f, yRot = 0.0f, zRot = 0.0f;	// rotational data
	GLfloat xScl = 1.0f, yScl = 1.0f, zScl = 1.0f;	// Scale data
	GLuint VBO, VAO, EBO; //vertex buffer object
	std::vector<GLfloat> vertices;
	std::vector<GLint> indices;
	glm::vec2 uvOffset;
	GLuint texture;
	glm::mat4 translation, rotation, scale;
	bool shouldBuffer = true;

	GameObject();
	void Animate(int frame);
	void GenerateBuffers();
	void BulletGenerateBuffers();
	void CoordVecCreation(GLfloat x, GLfloat y, GLfloat z, GLfloat texX, GLfloat texY);
	void BulletCoordVecCreation(GLfloat x, GLfloat y, GLfloat z);
	void Render(GLuint shaderProgram, glm::mat4 projectionMat, glm::mat4 viewMat);
	void BulletRender(GLuint shaderProgram, glm::mat4 projectionMat, glm::mat4 viewMat);
	bool CollisionCheck(GLfloat x, GLfloat y);
	void Move(GLfloat x, GLfloat y, GLfloat z);
	void Rotate(GLfloat xRot, GLfloat yRot, GLfloat zRot);
	void Scale(GLfloat xScl, GLfloat yScl, GLfloat zScl);
	~GameObject();

	

private:

};

