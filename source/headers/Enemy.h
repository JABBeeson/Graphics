#pragma once
#include "GameObject.h"
#include <iostream>
#include <ctime>

class Enemy : public GameObject
{
public:
	double moveAmount = 0.6f, dyingTimer = 0.0f;
	bool alive = true, moveRight = true, decend = false, remove = false;

	Enemy();
	Enemy(GLfloat width, GLfloat height);
	bool DoShoot();
	bool DoDecend(GLfloat deltaTime);
	bool DoMove(GLfloat deltaTime);
	~Enemy();
};