#pragma once

#include <Bengine/Window.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>

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

	///  Member variables
	Bengine::Window _window;					///< The game window
	Bengine::GLSLProgram _textureProgram;		///< The shader program
	Bengine::InputManager _inputManager;		///< Handles input
	Bengine::Camera2D _camera;					///< Main camera
};

