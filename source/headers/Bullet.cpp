#include "Bullet.h"

Bullet::Bullet() { }
Bullet::Bullet(GLfloat xPos, GLfloat yPos, GLfloat _width, GLfloat _height) {

	this->x = xPos;
	this->y = yPos;
	this->width = _width;
	this->height = _height;

	// fills vertices with that width and height

	// spawn in the center of the game world.
	//top left
	CoordVecCreation((0.0f - width / 2), (0.0f + height / 2), 0, 0.0f, 1.0f);

	//top right
	CoordVecCreation((0.0f + width / 2), (0.0f + height / 2), 0, 1.0f, 1.0f);

	//bottom left
	CoordVecCreation((0.0f - width / 2), (0.0f - height / 2), 0, 0.0f, 0.0f);

	//bottom right
	CoordVecCreation((0.0f + width / 2), (0.0f - height / 2), 0, 1.0f, 0.0f);

	// fills indices
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

}



void Bullet::DoMove(GLfloat deltaTime) {
	Move(0.0f, moveAmount * deltaTime, 0.0f);	
	// remove bullet if out of range of screen
	if (y > 1.5f) alive = false;
}

Bullet::~Bullet()
{
}
