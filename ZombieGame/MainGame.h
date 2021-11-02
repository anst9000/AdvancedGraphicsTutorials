#pragma once

#include <Bengine/Bengine.h>
#include <Bengine/Camera2D.h>
#include <Bengine/Errors.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/Timing.h>
#include <Bengine/Window.h>


#include "Player.h"
#include "Level.h"
#include "Bullet.h"

class Zombie;

enum class GameState
{
	PLAY,
	EXIT
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

	/// Initializes the level and sets up everything
	void initLevel();

	/// Initializes the player
	void initPlayer();

	/// Initializes the shaders
	void initShaders();

	/// Main game loop for the program
	void gameLoop();

	///  Update all agents
	void updateAgents( float deltaTime );

	/// Update bullets
	void updateBullets( float deltaTime );

	/// Check victory conditions
	void checkVictory();

	/// Handles input processing
	void processInput();

	/// Renders the game
	void drawGame();

	/// Draw the HUD
	void drawHud();

	///  Member variables
	Bengine::Window _window;					///< The game window
	Bengine::GLSLProgram _textureProgram;		///< The shader program
	Bengine::InputManager _inputManager;		///< Handles input
	Bengine::Camera2D _camera;					///< Main camera
	Bengine::Camera2D _hudCamera;				///< Hud camera
	Bengine::SpriteBatch _agentSpriteBatch;		///< Draws all agents
	Bengine::SpriteBatch _hudSpriteBatch;		///< Draws

	Level* _level;								///< Pointer to loaded level

	int _screenWidth;
	int _screenHeight;
	float _fps;

	std::vector<Human*> _humans;				///< Vector of all humans
	std::vector<Zombie*> _zombies;				///< Vector of all zombies
	std::vector<Bullet> _bullets;				///< Vector of all bullets
	Player* _player;							///< Pointer to the player

	int _currentLevel;

	int _numHumansKilled;						///< Humans killed by player
	int _numZombiesKilled;						///< Zombies killed by player

	Bengine::SpriteFont* _spriteFont;			///< Pointer to the SpriteFont

	GameState _gameState;
};

