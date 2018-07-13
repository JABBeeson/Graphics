#include "Enemy.h"

Enemy::Enemy() { } 
Enemy::Enemy(GLfloat _width, GLfloat _height) {
	// set width and height
	this->width = _width;
	this->height = _height;

	// spawn in the center of the game world.
	//top left
	CoordVecCreation((0.0f - width / 2),(0.0f + height / 2), 0, 0.0f, 1.0f);
	
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

bool Enemy::DoShoot(){
	std::srand(std::time(0));
	int shootDieRoll = std::rand()% 100;
	if (shootDieRoll < 10) {
		std::cout << "shoot" << std::endl;
		return false;
	}
	return false;
}

bool Enemy::DoDecend(GLfloat deltaTime) {
	switch (moveRight)
	{
	case true:
		if ((moveAmount * deltaTime) + x > 2.0f - width/2 - 0.05f)
			return true;	// decend
		break;

	case false:
		if ((-moveAmount * deltaTime) + x < -2.0f + width/2 + 0.05f)
			return true;	// decend
		break;
	}
	return false;
}

bool Enemy::DoMove(GLfloat deltaTime) {
	/*check if its going to be outside the boundry, if it is dont do the calculation.*/
	if (decend) {
			decend = false;
			Move(0, -0.2f, 0);
			moveRight = moveRight ? false : true;
			this->moveAmount += 0.1f;
			return false;
	}
	if (DoDecend(deltaTime)) {
		if (!moveRight)	Move(-moveAmount * deltaTime, 0, 0);
		else Move(moveAmount * deltaTime, 0, 0);
		return true;
	}

	//^^^^^^if decend, then abort^^^^^^
	if (!moveRight)	Move(-moveAmount * deltaTime, 0, 0);
	else Move(moveAmount * deltaTime, 0, 0);
	

	return false;
}


Enemy::~Enemy()
{
}
