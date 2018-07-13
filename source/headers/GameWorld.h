#pragma once
#include <vector>
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "EnemyBullet.h"
#include "ScoreDigit.h"
#include "Boundry.h"

class GameWorld
{
public:
	double deltaTimeCounter;
	bool enemyShootPermission = false;
	int playerScore = 0;
	int playerLives = 3;
	GLuint bulletTexture;
	Player player, background, foreground;
	std::vector<Boundry> borders;
	std::vector<Boundry> barricades;
	std::vector<Enemy> enemies;
	std::vector<EnemyBullet> EnemyBullets;
	std::vector<Bullet> Bullets;
	std::vector<Player> lifeIcons;
	std::vector<ScoreDigit> scoreDigitsOutput;
	std::vector<ScoreDigit> scoreDigitsItemList;
	
	GameWorld();
	void Clear();
	void EnemyShoot(GLfloat x, GLfloat y);
	void Shoot(GameObject gameObject);
	~GameWorld();

private:

};

