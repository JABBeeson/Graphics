#pragma once
#include "GameObject.h"

class ScoreDigit : public GameObject
{
public:
	ScoreDigit();
	ScoreDigit(GLfloat width, GLfloat height);
	~ScoreDigit();
};