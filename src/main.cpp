// https://learnopengl.com/#!Getting-started/Textures
// https://blackboard.lincoln.ac.uk/bbcswebdav/pid-1295330-dt-content-rid-2786800_2/courses/CGP2012M-1617/CGP2012M%20Texturing%20Workshop%205.pdf
// https://blackboard.lincoln.ac.uk/bbcswebdav/pid-1295327-dt-content-rid-2783338_2/courses/CGP2012M-1617/main_EBO_multipleTextures.cpp
// https://blackboard.lincoln.ac.uk/bbcswebdav/pid-1295327-dt-content-rid-2783332_2/courses/CGP2012M-1617/CGP2012M%20L05%20Texturing.pdf


#include <iostream>
#include <math.h>
#define _USE_MATH_DEFINES

// GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// NOTE: include before SDL.h
#include <GL/glew.h>

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

// - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// NOTE: must do before including GLM headers
// NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

const GLuint WIDTH = 800, HEIGHT = 600;
bool hasChanged = true, fulScrTog = false;

const GLchar* vertexShaderSource = {
	"#version 440 core\n"
	"layout(location=0) in vec3 position;\n"
	"layout (location = 1) in vec2 texCoord;\n"
	"out vec2 TexCoord;\n"
	"void main()\n"
	"{ \n"
			"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
			"TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);"
	"}"
};

const GLchar* fragmentShaderSource = {
	"#version 440 core\n"
	"in vec2 TexCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D Texture;\n"
	"void main()\n"
	"{ \n"
			"color = texture(Texture, TexCoord);\n"		
	"}" 
};

// gets full path
char* getFullPath(char* path) {
	char* fullPath = SDL_GetBasePath();
	strcat(fullPath, path);
	return fullPath;
}

// toggle fullscreen routine
void tFullScr(SDL_Window* window) {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;//unidentified 32 bit number
	bool fulScrTog = SDL_GetWindowFlags(window) & FullscreenFlag;	// determins wearther or no the window is toggled to full screen.
																	//cout << FullscreenFlag;

	SDL_SetWindowFullscreen(window, fulScrTog ? 0 : FullscreenFlag); // '?' condional notaion
}

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
	SDL_Window *win = nullptr;
	win = SDL_CreateWindow("Joseph Beeson - 15564479 : Graphics 01", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
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

	//***** LINKING THE VERTEX ATTRIBUTES ******

	GLfloat vertices[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // Top Right
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom Left
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f // Top Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3 // Second Triangle
	};

	GLfloat texCoords[] = {
		0.0f, 0.0f,  // Lower-left corner  
		1.0f, 0.0f,  // Lower-right corner
		0.5f, 1.0f   // Top-center corner
	};
	
	//	Image Loading 
	SDL_Surface* image = IMG_Load(getFullPath("assets/texture1.png"));
	if (image == NULL)
	{
		std::cout << "Image load failed...." << std::endl;
	}
	
	//Texture generation
	GLuint texture;
	glGenTextures(1, &texture);	// generate one texture
	glBindTexture(GL_TEXTURE_2D, texture);
	//Texture Wrapping/prameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Texture generation continued
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	// free image memory
	glBindTexture(GL_TEXTURE_2D, 0);	
	SDL_FreeSurface(image);	

	// triangle colour, border and fill.
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };	
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	GLuint VBO, VAO, EBO; //vertex buffer object
	bool gameRunning = true;

	while (gameRunning)
	{
		if (hasChanged) {
			//Set OpenGL buffers
			glGenBuffers(1, &VBO);
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &EBO);
			// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(
				GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW
			);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW
			);
			// Position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			// Texture attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			// Unbind VAO
			glBindVertexArray(0);

			hasChanged = false;
		}

		// Render
		// Clear the back buffer
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw our first triangle
		glUseProgram(shaderProgram);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Present the back buffer
		SDL_GL_SwapWindow(win);

	}

	// Wait
	SDL_Delay(2000);

	// Clean up
	SDL_Log("Finished. Cleaning up and closing down\n");

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
	return 0;
}


/*// Load the music
Mix_Music *music = Mix_LoadMUS("assets/beat.wav");
if (!music) {
SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Mix_LoadMUS error: %s\n",
Mix_GetError());
return 1;
}
SDL_Log("SDL_mixer loaded music OK!\n");

// Load the sound effect
Mix_Chunk *scratch = Mix_LoadWAV("assets/scratch.wav");
if (!scratch) {
SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Mix_LoadWAV error: %s\n",
Mix_GetError());
return 1;
}
SDL_Log("SDL_mixer loaded sound effect OK!\n");

// Play the music and sound effect
Mix_PlayChannel(-1, scratch, 0);
Mix_PlayMusic(music, -1);
Mix_FreeChunk(scratch);
Mix_FreeMusic(music);
Mix_CloseAudio();


// SDL_ttf initialise
if (TTF_Init() == -1) {
SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf init error: %s\n",
SDL_GetError());
return 1;
}
SDL_Log("SDL_ttf initialised OK!\n");

// SDL_mixer initialise
if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
"SDL_mixer init error: %s\n   PERHAPS NO "
"HEADPHONES/SPEAKERS CONNECTED\n",
Mix_GetError());
return 1;
}
SDL_Log("SDL_mixer initialised OK!\n");*/