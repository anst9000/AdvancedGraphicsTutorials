#include "MainGame.h"
#include <SDL/SDL.h>


MainGame::MainGame()
{
	// Empty
}

MainGame::~MainGame()
{
	// Empty
}

/// Runs the game
void MainGame::run()
{
	// Empty
}

/// Initializes the core systems
void MainGame::initSystems()
{
	// Empty
}

/// Initializes the shaders
void MainGame::initShaders()
{
	// Compile our color shader
	_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	_textureProgram.addAttribute("vertexPosition");
	_textureProgram.addAttribute("vertexColor");
	_textureProgram.addAttribute("vertexUV");
	_textureProgram.linkShaders();
}

/// Main game loop for the program
void MainGame::gameLoop()
{
	// Empty
}

/// Handles input processing
void MainGame::processInput()
{
	SDL_Event evnt;

	// Will keep looping unntil there are no more events to process
	while (SDL_PollEvent(&evnt))
	{
		// Check what type of event we get
		switch (evnt.type)
		{
		case SDL_QUIT:
			// Exit the game here
			break;
		case SDL_MOUSEMOTION:
			_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_inputManager.pressKey(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputManager.releaseKey(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_inputManager.pressKey(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputManager.releaseKey(evnt.button.button);
			break;
		}
	}
}

/// Renders the game
void MainGame::drawGame()
{
	// Set the base depth to 1.0
	glClearDepth(1.0);

	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw code goes here

	// Swap our buffer and draw everything to the screen
	_window.swapBuffer();
}
