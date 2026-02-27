#include <iostream>
#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "RayRenderer.hpp"
#include "FrameRenderer.hpp"

constexpr float PI = 3.14159265359f;

static float posX = 0, posY = 0, posZ = 0;
static float rotX = 0, rotY = 0;

static float velRotX = 0, velRotY = 0;

static bool inputPosX = false;
static bool inputNegX = false;
static bool inputPosY = false;
static bool inputNegY = false;
static bool inputPosZ = false;
static bool inputNegZ = false;

static unsigned int quadBuffer, quadVao;

static const int WINDOW_WIDTH = 600;
static const int WINDOW_HEIGHT = 600;
static const int RENDER_WIDTH = 600;
static const int RENDER_HEIGHT = 600;
static const float WINDOW_ASPECT = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;

static const float TURN_SPEED = 0.15;
static const float MOVE_SPEED = 5;

static const GLfloat QUAD_VERTEX_DATA[] = {
		-1, -1, 0, 0,
		-1, +1, 0, 1,
		+1, +1, 1, 1,
		-1, -1, 0, 0,
		+1, -1, 1, 0,
		+1, +1, 1, 1
};

static void ProcessKeyEvent(SDL_KeyboardEvent& ev) {
	if (ev.down) {
		switch (ev.key) {
		case SDLK_ESCAPE:
			exit(0);
			break;
		}
	}

	switch (ev.key) {
	case SDLK_ESCAPE:
		exit(0);
		break;
	case SDLK_W:
		inputNegZ = ev.down;
		break;
	case SDLK_S:
		inputPosZ = ev.down;
		break;
	case SDLK_A:
		inputPosX = ev.down;
		break;
	case SDLK_D:
		inputNegX = ev.down;
		break;
	case SDLK_Q:
		inputPosY = ev.down;
		break;
	case SDLK_E:
		inputNegY = ev.down;
		break;
	}
}

static void ProcessMouseMotion(SDL_MouseMotionEvent& ev) {
	velRotX += ev.xrel * WINDOW_ASPECT;
	velRotY += ev.yrel * WINDOW_ASPECT;
}

static void ProcessEvent(SDL_Event& ev) {
	switch (ev.type) {
	case SDL_EVENT_QUIT:
		exit(0);
		break;
	case SDL_EVENT_KEY_DOWN:
	case SDL_EVENT_KEY_UP:
		ProcessKeyEvent(ev.key);
		break;
	case SDL_EVENT_MOUSE_MOTION:
		ProcessMouseMotion(ev.motion);
		break;
	}
}

static void PollEvents(void) {
	// Reset relative velocities for frame
	velRotX = velRotY = 0;

	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		ProcessEvent(ev);
	}
}

int ExitCallback(void) {
	glDeleteBuffers(1, &quadBuffer);
	glDeleteVertexArrays(1, &quadVao);

	SDL_Quit();
	return 0;
}

int main(char** argc, int argv) {
	srand(time(NULL));

	SDL_Init(SDL_INIT_VIDEO);

	onexit(ExitCallback);

	// OpenGL 4.3 Core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL;
	SDL_Window* window = SDL_CreateWindow("Window", WINDOW_WIDTH, WINDOW_HEIGHT, windowFlags);
	if (window == nullptr) {
		std::cout << "Failed to create window\n";
		exit(1);
	}

	SDL_GLContext ctx = SDL_GL_CreateContext(window);
	if (ctx == nullptr) {
		std::cout << "Failed to create context\n";
		exit(1);
	}
	SDL_GL_MakeCurrent(window, ctx);

	if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
		std::cout << "Failed to load OpenGL\n";
		exit(1);
	}

	SDL_SetWindowRelativeMouseMode(window, true);

	glGenBuffers(1, &quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTEX_DATA), QUAD_VERTEX_DATA, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &quadVao);
	glBindVertexArray(quadVao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	RayRenderer rayRenderer(RENDER_WIDTH, RENDER_HEIGHT, quadVao);
	FrameRenderer frameRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, quadVao);

	Uint32 lastTick = SDL_GetTicks();
	// Ensure deltaTime is greater than 0
	SDL_Delay((rand() % 3) * 3);
	Uint32 currTick = SDL_GetTicks();

	while (true) {
		PollEvents();

		float deltaTime = (float)(currTick - lastTick) * 0.001;
		lastTick = currTick;
		currTick = SDL_GetTicks();

		// velRotX is already corrected for framerate by the way sdl events work
		rotY += velRotX / (2 * PI) * TURN_SPEED;
		rotX += velRotY / (2 * PI) * TURN_SPEED;

		rotX = fmin(fmax(rotX, -PI / 2.), PI / 2.);

		float cy = cos(rotY);
		float sy = sin(rotY);

		float localVelX = (inputPosX ? +1 : 0) + (inputNegX ? -1 : 0);
		float localVelY = (inputPosY ? +1 : 0) + (inputNegY ? -1 : 0);
		float localVelZ = (inputPosZ ? +1 : 0) + (inputNegZ ? -1 : 0);

		// X
		posX += -cy * localVelX * MOVE_SPEED * deltaTime;
		posZ += -sy * localVelX * MOVE_SPEED * deltaTime;
		// Y
		posY += localVelY * MOVE_SPEED * deltaTime;
		// Z
		posX += -sy * localVelZ * MOVE_SPEED * deltaTime;
		posZ += cy * localVelZ * MOVE_SPEED * deltaTime;

		rayRenderer.SetCameraPosition(posX, posY, posZ);
		rayRenderer.SetCameraRotation(rotX, rotY);

		rayRenderer.Render();
		frameRenderer.Render(rayRenderer.GetFrameTexture());

		SDL_GL_SwapWindow(window);
	}

	return 0;
}