#include "MainGame.h"
#include <Bengine/Errors.h>
#include <Bengine/Bengine.h>
#include <Bengine/ResourceManager.h>

#include <iostream>
#include <string>

MainGame::MainGame() :
	_screenWidth(950),
	_screenHeight(650),
	_gameState(GameState::PLAY),
	_time(0.0f),
	_fps(0.0f),
	_bullets(),
	_maxFPS(60.0f),
	_spriteBatch(),
	_inputManager(),
	_fpsLimiter()
{
	_camera.init(_screenWidth, _screenHeight);
}

MainGame::~MainGame()
{
}

// This runs the game
void MainGame::run()
{
	initSystems();

	// This only returns when the game ends
	gameLoop();
}

// Initialize SDL and OpenGL and whatever else we need
void MainGame::initSystems()
{
	Bengine::init();

	_window.create("Game Engine", _screenWidth, _screenHeight, 0);

	initShaders();

	_spriteBatch.init();
	_fpsLimiter.init(_maxFPS);
}

void MainGame::initShaders()
{
	_colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

void MainGame::gameLoop()
{
	// Will loop until we set _gameState to EXIT
	while (_gameState != GameState::EXIT)
	{
		_fpsLimiter.begin();

		processInput();
		_time += 0.01;

		_camera.update();

		for (int i = 0; i < _bullets.size();)
		{
			// Update all bullets
			if (_bullets[i].update())
			{
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
			}
			else
			{
				i++;
			}
		}

		drawGame();

		_fps = _fpsLimiter.end();

		// Print only once every 10th frame
		static int frameCounter = 0;
		frameCounter++;

		if (frameCounter == 10000)
		{
			std::cout << _fps << std::endl;
			frameCounter = 0;
		}
	}
}

// Processes input with SDL
void MainGame::processInput()
{
	SDL_Event evnt;

	const float CAMERA_SPEED = 2.0f;
	const float SCALE_SPEED = 0.1f;

	// Will keep looping unntil there are no more events to process
	while (SDL_PollEvent(&evnt))
	{
		// Check what type of event we get
		switch (evnt.type)
		{
			case SDL_QUIT:
				_gameState = GameState::EXIT;
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
			case SDL_MOUSEMOTION:
				_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
				break;
		}
	}

	if (_inputManager.isKeyDown(SDLK_UP))
	{
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0f, CAMERA_SPEED));
	}

	if (_inputManager.isKeyDown(SDLK_DOWN))
	{
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0f, -CAMERA_SPEED));
	}

	if (_inputManager.isKeyDown(SDLK_LEFT))
	{
		_camera.setPosition(_camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));
	}

	if (_inputManager.isKeyDown(SDLK_RIGHT))
	{
		_camera.setPosition(_camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
	}

	if (_inputManager.isKeyDown(SDLK_z))
	{
		_camera.setScale(_camera.getScale() + SCALE_SPEED);
	}

	if (_inputManager.isKeyDown(SDLK_x))
	{
		_camera.setScale(_camera.getScale() - SCALE_SPEED);
	}

	if (_inputManager.isKeyDown(SDL_BUTTON_LEFT))
	{
		glm::vec2 mouseCoords = _inputManager.getMouseCoords();
		mouseCoords = _camera.convertScreenToWorld(mouseCoords);

		glm::vec2 playerPosition(0.0f);
		glm::vec2 direction = mouseCoords - playerPosition;
		direction = glm::normalize(direction);

		_bullets.emplace_back(playerPosition, direction, 5.0f, 1000);
	}

}

void MainGame::drawGame()
{
	glClearDepth(1.0);
	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable the shader
	_colorProgram.use();

	glActiveTexture(GL_TEXTURE0);

	GLint textureLocation = _colorProgram.getUniformLocation( "playerSampler" );
	glUniform1i( textureLocation, 0 );

	// Set the camera matrix
	GLint pLocation = _colorProgram.getUniformLocation( "P" );
	glm::mat4 cameraMatrix = _camera.getCameraMatrix();

	glUniformMatrix4fv( pLocation, 1, GL_FALSE, &( cameraMatrix[ 0 ][ 0 ] ) );

	_spriteBatch.begin();

	glm::vec4 pos(0.0f, 0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Textures/PNG/CharacterRight_Standing.png");
	Bengine::ColorRGBA8 color( 255, 255, 255, 255 );

	_spriteBatch.draw(pos, uv, texture.id, 0.0f, color);

	for (int i = 0; i < _bullets.size(); i++)
	{
		_bullets[i].draw(_spriteBatch);
	}

	_spriteBatch.end();
	_spriteBatch.render();

	glBindTexture(GL_TEXTURE_2D, 0);

	_colorProgram.unuse();

	// Swap our buffer and draw everything to the screen
	_window.swapBuffer();
}
