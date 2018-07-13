#pragma once
#include "GameObject.h"
#include "Player.h"

class Bullet : public GameObject
{
public:
	double moveAmount = 1.0f;
	bool alive = true;

	Bullet();
	Bullet(GLfloat xPos, GLfloat yPos, GLfloat width, GLfloat height);
	void DoMove(GLfloat deltaTime);
	~Bullet();
};

