#pragma once
#include "GameObject.h"

class Boundry : public GameObject
{
public:
	int condition = 2;
	bool alive = true;
	Boundry();
	Boundry(GLfloat width, GLfloat height);
	~Boundry();
};

