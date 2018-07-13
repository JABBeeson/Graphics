#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#define _USE_MATH_DEFINES

// GLEW - OpenGL Extension Wrangler 
#include <GL/glew.h>

// SDL - Simple DirectMedia Layer 
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#define GLM_FORCE_RADIANS // force glm to use radians
// NOTE: must do before including GLM headers
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//importing header files
//#include "headers\Globals.h"
#include "headers\GameWorld.h"
#include "headers\GameObject.h"

using namespace std;
using namespace chrono;

GameWorld gameWorld;

SDL_Window *win = nullptr;
const GLuint WIDTH = 800, HEIGHT = 600;
bool running = true, fulScrTog = false;

//initialising functions
void processInput();
void update(double deltaTime);
void render(GLuint shaderProgram, glm::mat4 projectionMat, glm::mat4 viewMat);
void reset();
void restart();
void generateGame();
void updateScore();
void toggleFullScreen();
void setWindowSize();
high_resolution_clock::time_point getCurrentTime();
char* getFullPath(char* path);

GLuint temp_texture;

const GLchar* vertexShaderSource = {
	"#version 440 core\n"
	"layout(location=0) in vec3 position;\n"
	"layout (location = 1) in vec2 texCoord;\n"
	"out vec2 TexCoord;\n"
	"uniform mat4 trans;\n"	//4x4 matrix for transformation
	"uniform vec2 uvoff;\n"
	"void main()\n"
	"{ \n"
			"gl_Position = trans * vec4(position.x, position.y, position.z, 1.0);\n"	//multiply the trans by positon vector for transformation
			"TexCoord = vec2(texCoord.x + uvoff.x, 1.0f - texCoord.y + uvoff.y);"
	"}"
};

const GLchar* fragmentShaderSource = {
	"#version 440 core\n"
	"in vec2 TexCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D Texture;\n"
	"void main()\n"
	"{ \n"
	"color = texture(Texture, TexCoord) * vec4(1,1,1,1);\n"		
	"}" 
};

int main(int argc, char *argv[]) {
	// SDL initialise
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n",
			SDL_GetError());
		return 1;
	}
	SDL_Log("SDL initialised OK!\n");

	// SDL_Image initialise
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_image init error: %s\n",
			IMG_GetError());
		return 1;
	}
	SDL_Log("SDL_image initialised OK!\n");

	// Window Creation
	win = nullptr;
	win = SDL_CreateWindow("Joseph Beeson - 15564479 : Graphics : 2D SpaceInvaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (win == nullptr) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"SDL_CreateWindow init error: %s\n", SDL_GetError());
		return 1;
	}

	// Set OpenGL context parameters
	int major = 4, minor = 3;
	SDL_Log("Asking for OpenGL %d.%d context\n", major, minor);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE); 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); 
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 

	// OpenGL Context Creation
	SDL_GLContext glcontext = SDL_GL_CreateContext(win);
	if (glcontext == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"SDL_GL_CreateContext init error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	SDL_Log("Got an OpenGL %d.%d context\n", major, minor);

	// initialise GLEW - sets up the OpenGL function pointers for the version of OpenGL we're using
	GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"glewInit error: %s\n", glewGetErrorString(rev));
		return 1;
	}
	SDL_Log("glew initialised OK!\n");

	// ***** Build and compile our shader program *****

	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);	// attaching the shaders
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//shader Deletion
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//projection init
	setWindowSize();
	glm::mat4 projectionMatrix, viewMatrix, modelMatrix;
	projectionMatrix = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, -1.0f, 100.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(2.0f, 1.5f, 0.0f));

	generateGame(); 
	double deltaTime = 0, playerMoveDis = 0; //time difference
	chrono::high_resolution_clock::time_point previous = getCurrentTime();

	while (running)
	{
		deltaTime = duration_cast<microseconds>(getCurrentTime() - previous).count() / 1000000.0;
		gameWorld.deltaTimeCounter += deltaTime;
		previous = getCurrentTime();

		processInput();
		
		//determin how much to move the camera
		playerMoveDis = gameWorld.player.x;	// I know its gross and really ganky, but it works

		update(deltaTime);
		
		//determin how much to move the camera
		playerMoveDis = playerMoveDis - gameWorld.player.x;

		viewMatrix = glm::translate(viewMatrix, glm::vec3(playerMoveDis * 0.5f, 0.0f, 0.0f));

		render(shaderProgram, projectionMatrix, viewMatrix);
	}

	// Clean up
	SDL_Log("Finished. Cleaning up and closing down\n");

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
	return 0;
}

void processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					setWindowSize();
				}
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						running = false;
						break;
					
					case SDLK_a:
						gameWorld.player.moveAmount = -1;
						break;
					
					case SDLK_d:
						gameWorld.player.moveAmount = 1;
						break;

					case SDLK_SPACE:
						gameWorld.Shoot(gameWorld.player);
						break;
				}
			break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
				case SDLK_a :
					gameWorld.player.moveAmount = 0;
					break;
				case SDLK_d:
					gameWorld.player.moveAmount = 0;
					break;
				case SDLK_o:
					toggleFullScreen();
					break;
				}
			break;
							
			case SDL_QUIT:
				running = false;
			break;
		}
	}
}

int lastState;

void update(double deltaTime)
{
	if (gameWorld.playerLives <= 0 || !gameWorld.player.alive) restart();
	gameWorld.player.DoMove(deltaTime);
	
	vector<Enemy>::iterator alienIT;
	vector<Bullet>::iterator BulletIT;
	vector<EnemyBullet>::iterator eBulletIT;
	vector<Boundry>::iterator barrierInt;
	bool decend = false, doAnimate = false;
	int animState = gameWorld.deltaTimeCounter * 2;
	animState %= 2;
	animState += 1;

	if (animState != lastState) doAnimate = true;

	gameWorld.background.uvOffset.y += deltaTime * 0.2;
	gameWorld.foreground.uvOffset.y += deltaTime * 0.6;

	//moves all aliens
	if (gameWorld.enemies.size() == 0) {
		reset();
	}

	for (alienIT = gameWorld.enemies.begin(); alienIT < gameWorld.enemies.end(); alienIT++) {
		if(doAnimate) {
			alienIT->Animate(animState);
		}
		if (alienIT->alive) {
			if (alienIT->DoMove(deltaTime)) {
				decend = true;
				gameWorld.enemyShootPermission = true;
			}
			if (alienIT->y - (alienIT->height/2) <= gameWorld.player.y){
				gameWorld.playerLives--;
				// loose game
			}
		}
		if (!alienIT->alive) {
			//Spin off code
			alienIT->dyingTimer += deltaTime;
			alienIT->Move(0, deltaTime * 0.5f, 0);
			alienIT->Rotate(deltaTime * -75.0f, deltaTime * -75.0f, deltaTime * -75.0f);
			alienIT->Scale(deltaTime * -0.75f, deltaTime * -0.75f, deltaTime * -0.75f);
			if (alienIT->dyingTimer >= 1.5f) alienIT->remove = true;
		}
		
	}
	if(decend){
		 
		for (alienIT = gameWorld.enemies.begin(); alienIT < gameWorld.enemies.end(); alienIT++) {
			if(alienIT->alive)		alienIT->decend = true;
		}
	}
	if(gameWorld.enemyShootPermission){
		int shootDieRoll = std::rand() % 300;
		if (shootDieRoll < gameWorld.enemies.size()) {
			gameWorld.EnemyShoot(gameWorld.enemies.at(shootDieRoll).x, gameWorld.enemies.at(shootDieRoll).y);	//only shoot if alive.
		}
	}
		
	//Player Bullet Collision with Enemys
	if (gameWorld.Bullets.size() >= 1)	// if there are any bullets do the following
	{
		for (BulletIT = gameWorld.Bullets.begin(); BulletIT < gameWorld.Bullets.end();) {
			BulletIT->DoMove(deltaTime);

			bool hitAlien = false;
			//check every alien per bullet, to see if collided
			for (alienIT = gameWorld.enemies.begin(); alienIT < gameWorld.enemies.end(); alienIT++) {
				if (alienIT->CollisionCheck(BulletIT->x, BulletIT->y) && alienIT->alive) {
					alienIT->alive = false;
					gameWorld.playerScore += 10;
					updateScore();
					hitAlien = true;
					break;
				}
				else {
					hitAlien = false;
				}
			}
			for (barrierInt = gameWorld.barricades.begin(); barrierInt < gameWorld.barricades.end(); barrierInt++) {
				if (barrierInt->CollisionCheck(BulletIT->x, BulletIT->y)) {
					barrierInt->condition--;
					BulletIT->alive = false;
				}
			}
			if (hitAlien || !BulletIT->alive) BulletIT = gameWorld.Bullets.erase(BulletIT);
			else BulletIT++;
		}
	}

	//Enemy Bullet Collision with the Player
	if (gameWorld.EnemyBullets.size() >= 1)	// if there are any bullets do the following
	{
		for (eBulletIT = gameWorld.EnemyBullets.begin(); eBulletIT < gameWorld.EnemyBullets.end();) {
			eBulletIT->DoMove(deltaTime);//move every bullet

			bool hitPlayer = false;
			//check every alien per bullet, to see if collided
			if (gameWorld.player.CollisionCheck(eBulletIT->x, eBulletIT->y)) {
				
				gameWorld.playerLives--;
				cout << "dead" << endl;
				hitPlayer = true;
			}
			else {
				hitPlayer = false;
			}
			for (barrierInt = gameWorld.barricades.begin(); barrierInt < gameWorld.barricades.end(); barrierInt++) {
				if (barrierInt->CollisionCheck(eBulletIT->x, eBulletIT->y)) {
					barrierInt->condition--;
					eBulletIT->alive = false;
				}
			}
			
			if (hitPlayer || !eBulletIT->alive) eBulletIT = gameWorld.EnemyBullets.erase(eBulletIT);
			else eBulletIT++;
		}
	}

	for (barrierInt = gameWorld.barricades.begin(); barrierInt < gameWorld.barricades.end();) {
		switch (barrierInt->condition)
		{
		case 0:
			barrierInt->alive = false;
			break;
		case 1:
			//set image to half broken
			barrierInt->Animate(1);
			break;
		case 2:
			//image is fine.
			barrierInt->Animate(2);
			break;		
		}
		if (!barrierInt->alive) barrierInt = gameWorld.barricades.erase(barrierInt);
		else barrierInt++;
	}

	for (alienIT = gameWorld.enemies.begin(); alienIT < gameWorld.enemies.end();) {
		if (alienIT->remove) alienIT = gameWorld.enemies.erase(alienIT);
		else alienIT++;
	}	

	lastState = animState;
}

void render(GLuint shaderProgram, glm::mat4 projectionMat, glm::mat4 viewMat)
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	gameWorld.background.Render(shaderProgram, projectionMat, viewMat);
	gameWorld.foreground.Render(shaderProgram, projectionMat, viewMat);

	vector<Boundry>::iterator BoundryIT;
	for (BoundryIT = gameWorld.borders.begin(); BoundryIT < gameWorld.borders.end(); BoundryIT++) {
		BoundryIT->Render(shaderProgram, projectionMat, viewMat);
	}

	gameWorld.player.Render(shaderProgram, projectionMat, viewMat);

	vector<Enemy>::iterator alienIT;
	for (alienIT = gameWorld.enemies.begin(); alienIT < gameWorld.enemies.end(); alienIT++) {
		alienIT->Render(shaderProgram, projectionMat, viewMat);
	}
	vector<EnemyBullet>::iterator eBulletIT;
	for (eBulletIT = gameWorld.EnemyBullets.begin(); eBulletIT < gameWorld.EnemyBullets.end(); eBulletIT++) {
		eBulletIT->Render(shaderProgram, projectionMat, viewMat);
	}
	vector<Bullet>::iterator BulletIT;
	for (BulletIT = gameWorld.Bullets.begin(); BulletIT < gameWorld.Bullets.end(); BulletIT++) {
		BulletIT->Render(shaderProgram, projectionMat, viewMat);
	}
	
	for (int iconCount = 0; iconCount < gameWorld.playerLives; iconCount++) {
		gameWorld.lifeIcons.at(iconCount).Render(shaderProgram, projectionMat, viewMat);
	}
	vector<ScoreDigit>::iterator scoreDigitsIT;
	for (scoreDigitsIT = gameWorld.scoreDigitsOutput.begin(); scoreDigitsIT < gameWorld.scoreDigitsOutput.end(); scoreDigitsIT++) {
		scoreDigitsIT->Render(shaderProgram, projectionMat, viewMat);
	}
	for (BoundryIT = gameWorld.barricades.begin(); BoundryIT < gameWorld.barricades.end(); BoundryIT++) {
		BoundryIT->Render(shaderProgram, projectionMat, viewMat);
	}
	
	SDL_GL_SwapWindow(win);
}

void reset() {
	generateGame();
}

void restart() {
	gameWorld.playerScore = 0;
	gameWorld.playerLives = 3;
	updateScore();
	generateGame();
}

void generateGame()
{	
	// Load images
	vector<SDL_Surface*> images;
	images.clear();
	images.push_back(IMG_Load(getFullPath("assets/textureEnemy.png")));// Alien
	images.push_back(IMG_Load(getFullPath("assets/texturePlayer.png")));// Player
	images.push_back(IMG_Load(getFullPath("assets/textureNum0.png")));// 0
	images.push_back(IMG_Load(getFullPath("assets/textureNum1.png")));// 1
	images.push_back(IMG_Load(getFullPath("assets/textureNum2.png")));// 2
	images.push_back(IMG_Load(getFullPath("assets/textureNum3.png")));// 3
	images.push_back(IMG_Load(getFullPath("assets/textureNum4.png")));// 4
	images.push_back(IMG_Load(getFullPath("assets/textureNum5.png")));// 5
	images.push_back(IMG_Load(getFullPath("assets/textureNum6.png")));// 6
	images.push_back(IMG_Load(getFullPath("assets/textureNum7.png")));// 7
	images.push_back(IMG_Load(getFullPath("assets/textureNum8.png")));// 8
	images.push_back(IMG_Load(getFullPath("assets/textureNum9.png")));// 9
	images.push_back(IMG_Load(getFullPath("assets/textureBoundrys.png")));// boundrys
	images.push_back(IMG_Load(getFullPath("assets/textureBarricade.png")));// barricades
	images.push_back(IMG_Load(getFullPath("assets/textureBackground.png")));// background
	images.push_back(IMG_Load(getFullPath("assets/textureForeground.png")));// foreground
	images.push_back(IMG_Load(getFullPath("assets/textureBullet.png")));// Bullet
	for (int i = 0; i < images.size(); i++) {
		if (images.at(i) == NULL)
		{
			std::cout << "Image load failed...." << std::endl;
		}
	}
	//texture Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// texture initialising 
	GLuint textures[17];	// array of textures
	glGenTextures(17, textures);
	for (int i = 0; i < 17; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[i]->w, images[i]->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);	//unbind texture
		SDL_FreeSurface(images[i]);
		cout << i << endl;
	}

	gameWorld.Clear();

	gameWorld.bulletTexture = textures[16];

	Player* fore = new Player(4.0f, 3.0f);
	fore->x = 0.0f;
	fore->y = 0.0f;
	fore->texture = textures[15];
	fore->moveAmount = 0.0;
	gameWorld.foreground = *fore;
	delete fore;

	Player* back = new Player(4.0f, 3.0f);
	back->x = 0.0f;
	back->y = 0.0f;
	back->texture = textures[14];
	back->moveAmount = 0.0;
	gameWorld.background = *back;
	delete back;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 3; j++) {
			// create alien 
			Enemy* e = new Enemy(0.2f, 0.2f);
			e->x = -2.0f + i * 0.2f;
			e->y = 1.5f - j * 0.2f;
			e->texture = textures[0];
			gameWorld.enemies.push_back(*e);
			delete e;
		}
	}
	//generate player
	Player* p = new Player(0.2f, 0.2f);
	p->x = 0.0f;
	p->y = -1.2f;
	p->texture = textures[1];
	gameWorld.player = *p;
	delete p;

	for (int i = 0; i < gameWorld.playerLives; i++) {
		Player* p = new Player(0.2f, 0.2f);
		p->x = -1.5f + (i * 0.3f);
		p->y = -1.33f;
		p->texture = textures[1];
		gameWorld.lifeIcons.push_back(*p);
		delete p;
	}
	for (int i = 0; i < 10; i++) {
		ScoreDigit* d = new ScoreDigit(0.1f, 0.2f);
		d->x = 0.0f;
		d->y = 0.0f;
		d->texture = textures[i+2];
		gameWorld.scoreDigitsItemList.push_back(*d);
		delete d;
	}
	//Barrcadies
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 20; j++) {
			Boundry* ba = new Boundry(0.1f, 0.1f);
			if (j < 5) {
				//first group
				ba->x = -1.9f + (j * 0.1f);
				ba->y = -1.0f + (i * 0.1f);
			}
			if (j < 10 && j >= 5) {
				//second group
				ba->x = -0.9f + ((j-5) * 0.1f);
				ba->y = -1.0f + (i * 0.1f);
			}
			if (j < 15 && j >= 10) {
				//third group
				ba->x = 0.1f + ((j - 10) * 0.1f);
				ba->y = -1.0f + (i * 0.1f);
			}
			if (j >= 15) {
				//forth group
				ba->x = 1.1f + ((j - 15) * 0.1f);
				ba->y = -1.0f + (i * 0.1f);
			}
			ba->texture = textures[13];
			gameWorld.barricades.push_back(*ba);
			delete ba;
		}
	}

	// left vertical border
	Boundry* b = new Boundry(0.1f, 3.0f);
	b->texture = textures[12];
	b->x = -2.0f;
	b->y = 0.0f;
	gameWorld.borders.push_back(*b);
	delete b;
	// bottom horizontal border
	b = new Boundry(4.0f, 0.1f);
	b->texture = textures[12];
	b->x = 0.0f;
	b->y = -1.5f;
	gameWorld.borders.push_back(*b);
	delete b;
	// right vertical border
	b = new Boundry(0.1f, 3.0f);
	b->texture = textures[12];
	b->x = 2.0f;
	b->y = 0.0f;
	gameWorld.borders.push_back(*b);
	delete b;

}


void updateScore() {
	gameWorld.scoreDigitsOutput.clear();
	int i = gameWorld.playerScore;
	cout << " score: " << i << endl;
	int length = 0;
	int digit;
	for (; i > 0; length++) {
		digit = i % 10;
		i = i / 10;
		gameWorld.scoreDigitsItemList.at(digit).y = -1.33f;
			//set x and y;
		gameWorld.scoreDigitsOutput.push_back(gameWorld.scoreDigitsItemList.at(digit));
		cout << digit << endl;
	}
	//cout << gameWorld.playerScore << " " << length << endl;
	vector<ScoreDigit>::iterator scoreDigitsIT;
	for (scoreDigitsIT = gameWorld.scoreDigitsOutput.begin(); scoreDigitsIT < gameWorld.scoreDigitsOutput.end(); scoreDigitsIT++) {

		scoreDigitsIT->x = 1.0f + (length * gameWorld.scoreDigitsItemList.at(length).width);
		length--;
	}
}

void toggleFullScreen() {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	bool fulScrTog = SDL_GetWindowFlags(win) & FullscreenFlag;
	SDL_SetWindowFullscreen(win, fulScrTog ? 0 : FullscreenFlag);
}

void setWindowSize() {
	int w, h, newWidth;
	w = SDL_GetWindowSurface(win)->w;
	h = SDL_GetWindowSurface(win)->h;
	newWidth = (h / 3.0f) * 4.0f;

	glViewport((w - newWidth) / 2, 0.0f, newWidth, h);
}

chrono::high_resolution_clock::time_point getCurrentTime() 
{
	return chrono::high_resolution_clock::now();
}

char* getFullPath(char* path) {
	char* fullPath = SDL_GetBasePath();
	strcat(fullPath, path);
	return fullPath;
}