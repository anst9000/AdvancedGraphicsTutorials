#pragma once

#include <Bengine/Bengine.h>
#include <Bengine/Camera2D.h>
#include <Bengine/BengineErrors.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/InputManager.h>
#include <Bengine/ResourceManager.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/SpriteFont.h>
#include <Bengine/Timing.h>
#include <Bengine/Window.h>
#include <Bengine/AudioEngine.h>

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
	Bengine::Window m_window;					///< The game window
	Bengine::GLSLProgram m_textureProgram;		///< The shader program
	Bengine::InputManager m_inputManager;		///< Handles input
	Bengine::Camera2D m_camera;					///< Main camera
	Bengine::Camera2D m_hudCamera;				///< Hud camera
	Bengine::SpriteBatch m_agentSpriteBatch;	///< Draws all agents
	Bengine::SpriteBatch m_hudSpriteBatch;		///< Draws

	Level* m_level = NULL;						///< Pointer to loaded level

	int m_screenWidth = 920;
	int m_screenHeight = 690;
	float m_fps = 0.0f;

	std::vector<Human*> m_humans;				///< Vector of all humans
	std::vector<Zombie*> m_zombies;				///< Vector of all zombies
	std::vector<Bullet> m_bullets;				///< Vector of all bullets
	Player* m_player = NULL;						///< Pointer to the player

	int m_currentLevel = 0;

	int m_numHumansKilled = 0;					///< Humans killed by player
	int m_numZombiesKilled = 0;					///< Zombies killed by player

	Bengine::SpriteFont* m_spriteFont = NULL;	///< Pointer to the SpriteFont
	Bengine::AudioEngine m_audioEngine;

	GameState m_gameState = GameState::PLAY;
};

