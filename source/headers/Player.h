#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	double moveAmount = 0.0f, bulletTimer = 0.0f;
	bool alive = true, moveRight = true;

	Player();
	Player(GLfloat width, GLfloat height);
	void DoMove(GLfloat deltaTime);
	~Player();
};

