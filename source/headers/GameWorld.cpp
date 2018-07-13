#include "GameWorld.h"
#include <iostream>

GameWorld::GameWorld()
{
}

void GameWorld::Clear() {
	this->enemies.clear();
	this->EnemyBullets.clear();
	this->Bullets.clear();
	this->enemyShootPermission = false;
	this->deltaTimeCounter = 0.0f;
}

void GameWorld::EnemyShoot(GLfloat x, GLfloat y)
{
	EnemyBullet* b = new EnemyBullet(x, y, 0.025f, 0.12f);
	b->texture = this->bulletTexture;
	EnemyBullets.push_back(*b);
	delete b;
}

void GameWorld::Shoot(GameObject gameObject)
{
	if(this->Bullets.size() <= 3){
		Bullet* b = new Bullet(gameObject.x, gameObject.y, 0.025f, 0.12f);
		b->texture = this->bulletTexture;
		Bullets.push_back(*b);
		delete b;
	}
}

GameWorld::~GameWorld()
{
}