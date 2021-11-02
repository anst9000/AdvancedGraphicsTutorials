#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <vector>

#include <Bengine/GLSLProgram.h>
#include <Bengine/Sprite.h>
#include <Bengine/GLTexture.h>
#include <Bengine/Window.h>
#include <Bengine/Camera2D.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/InputManager.h>
#include <Bengine/Timing.h>

#include "Bullet.h"

enum class GameState
{
	PLAY, EXIT
};

class MainGame
{
public:
	MainGame();
	~MainGame();

	/// Runs the game
	void run();

private:
	/// Initializes the core systems
	void initSystems();

	/// Initializes the shaders
	void initShaders();

	/// Main game loop for the program
	void gameLoop();

	/// Handles input processing
	void processInput();

	/// Renders the game
	void drawGame();

	Bengine::Window m_window;
	int m_screenWidth;
	int m_screenHeight;

	GameState m_gameState;

	Bengine::GLSLProgram _colorProgram;
	Bengine::Camera2D m_camera;

	Bengine::SpriteBatch _spriteBatch;

	Bengine::InputManager m_inputManager;
	Bengine::FPSLimiter _fpsLimiter;

	std::vector<Bullet> m_bullets;

	float m_maxFPS;
	float m_fps;
	float _time;
};

