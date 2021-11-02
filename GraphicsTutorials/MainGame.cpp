#include "MainGame.h"
#include <Bengine/Errors.h>
#include <Bengine/Bengine.h>
#include <Bengine/ResourceManager.h>

#include <iostream>
#include <string>

MainGame::MainGame() :
	m_screenWidth(950),
	m_screenHeight(650),
	m_gameState(GameState::PLAY),
	_time(0.0f),
	m_fps(0.0f),
	m_bullets(),
	m_maxFPS(60.0f),
	_spriteBatch(),
	m_inputManager(),
	_fpsLimiter()
{
	m_camera.init(m_screenWidth, m_screenHeight);
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

	m_window.create("Game Engine", m_screenWidth, m_screenHeight, 0);

	initShaders();

	_spriteBatch.init();
	_fpsLimiter.init(m_maxFPS);
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
	while (m_gameState != GameState::EXIT)
	{
		_fpsLimiter.begin();

		processInput();
		_time += 0.01;

		m_camera.update();

		for (int i = 0; i < m_bullets.size();)
		{
			// Update all bullets
			if (m_bullets[i].update())
			{
				m_bullets[i] = m_bullets.back();
				m_bullets.pop_back();
			}
			else
			{
				i++;
			}
		}

		drawGame();

		m_fps = _fpsLimiter.end();

		// Print only once every 10th frame
		static int frameCounter = 0;
		frameCounter++;

		if (frameCounter == 10000)
		{
			std::cout << m_fps << std::endl;
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
				m_gameState = GameState::EXIT;
				break;
			case SDL_KEYDOWN:
				m_inputManager.pressKey(evnt.key.keysym.sym);
				break;
			case SDL_KEYUP:
				m_inputManager.releaseKey(evnt.key.keysym.sym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				m_inputManager.pressKey(evnt.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				m_inputManager.releaseKey(evnt.button.button);
				break;
			case SDL_MOUSEMOTION:
				m_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
				break;
		}
	}

	if (m_inputManager.isKeyDown(SDLK_UP))
	{
		m_camera.setPosition(m_camera.getPosition() + glm::vec2(0.0f, CAMERA_SPEED));
	}

	if (m_inputManager.isKeyDown(SDLK_DOWN))
	{
		m_camera.setPosition(m_camera.getPosition() + glm::vec2(0.0f, -CAMERA_SPEED));
	}

	if (m_inputManager.isKeyDown(SDLK_LEFT))
	{
		m_camera.setPosition(m_camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));
	}

	if (m_inputManager.isKeyDown(SDLK_RIGHT))
	{
		m_camera.setPosition(m_camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
	}

	if (m_inputManager.isKeyDown(SDLK_z))
	{
		m_camera.setScale(m_camera.getScale() + SCALE_SPEED);
	}

	if (m_inputManager.isKeyDown(SDLK_x))
	{
		m_camera.setScale(m_camera.getScale() - SCALE_SPEED);
	}

	if (m_inputManager.isKeyDown(SDL_BUTTON_LEFT))
	{
		glm::vec2 mouseCoords = m_inputManager.getMouseCoords();
		mouseCoords = m_camera.convertScreenToWorld(mouseCoords);

		glm::vec2 playerPosition(0.0f);
		glm::vec2 direction = mouseCoords - playerPosition;
		direction = glm::normalize(direction);

		m_bullets.emplace_back(playerPosition, direction, 5.0f, 1000);
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
	glm::mat4 cameraMatrix = m_camera.getCameraMatrix();

	glUniformMatrix4fv( pLocation, 1, GL_FALSE, &( cameraMatrix[ 0 ][ 0 ] ) );

	_spriteBatch.begin();

	glm::vec4 pos(0.0f, 0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Textures/PNG/CharacterRight_Standing.png");
	Bengine::ColorRGBA8 color( 255, 255, 255, 255 );

	_spriteBatch.draw(pos, uv, texture.id, 0.0f, color);

	for (int i = 0; i < m_bullets.size(); i++)
	{
		m_bullets[i].draw(_spriteBatch);
	}

	_spriteBatch.end();
	_spriteBatch.render();

	glBindTexture(GL_TEXTURE_2D, 0);

	_colorProgram.unuse();

	// Swap our buffer and draw everything to the screen
	m_window.swapBuffer();
}
