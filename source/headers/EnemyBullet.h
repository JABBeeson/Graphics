#pragma once
#include "GameObject.h"
#include "Enemy.h"

class EnemyBullet : public GameObject
{
public:
	double moveAmount = -1.0f;
	bool alive = true;

	EnemyBullet();
	EnemyBullet(GLfloat xPos, GLfloat yPos, GLfloat width, GLfloat height);
	void DoMove(GLfloat deltaTime);
	~EnemyBullet();
};

