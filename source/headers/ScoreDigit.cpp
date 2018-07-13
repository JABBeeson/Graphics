#include "ScoreDigit.h"

ScoreDigit::ScoreDigit(){}
ScoreDigit::ScoreDigit(GLfloat _width, GLfloat _height) {
	// fills vertices with that width and height
	this->width = _width;
	this->height = _height;

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
ScoreDigit::~ScoreDigit() {}