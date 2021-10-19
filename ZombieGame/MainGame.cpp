#include "MainGame.h"

#include <SDL/SDL.h>
#include <Bengine/Bengine.h>
#include <Bengine/Timing.h>
#include <Bengine/Errors.h>

#include "Zombie.h"
#include "Gun.h"

#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

MainGame::MainGame() :
	_screenWidth( 960 ),
	_screenHeight( 680 ),
	_fps( 0.0f ),
	_humans(),
	_zombies(),
	_player( nullptr ),
	_currentLevel( 0 ),
	_numHumansKilled( 0 ),
	_numZombiesKilled( 0 ),
	_gameState(GameState::PLAY)
{
	// Empty
}

MainGame::~MainGame()
{
	delete _level;

	// Delete all humans remaining
	std::for_each( _humans.begin(), _humans.end(), [&]( Human* h ) { delete h; } );

	// Delete all zombies remaining
	std::for_each( _zombies.begin(), _zombies.end(), [&]( Zombie* z ) { delete z; } );
}

/// Runs the game
void MainGame::run()
{
	initSystems();
	initLevel();
	initPlayer();

	gameLoop();
}

/// Initializes the core systems
void MainGame::initSystems()
{
	Bengine::init();

	_window.create("Zombie Game", _screenWidth, _screenHeight, 0);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	initShaders();

	_agentSpriteBatch.init();

	_camera.init(_screenWidth, _screenHeight);
}

void MainGame::initLevel()
{
	// Level 1
	_currentLevel = 1;
	_level = new Level("Levels/level1.txt");

	std::cout << "Current level " << _currentLevel << std::endl;
}

void MainGame::initPlayer()
{
	_player = new Player();
	_player->init( PLAYER_SPEED, _level->getPlayerPosition(), &_inputManager, &_camera, &_bullets);

	_humans.push_back(_player);

	std::cout << _level->getWidth() - 1 << "\n" << _level->getHeight() - 1 << std::endl;

	std::mt19937 randomEngine;
	randomEngine.seed( time( nullptr ) );
	std::uniform_int_distribution<int> randX( 2, _level->getWidth() - 2 );
	std::uniform_int_distribution<int> randY( 2, _level->getHeight() - 2 );

	// Add humans at random positions
	for ( int i = 0; i < _level->getNumHumans(); i++ )
	{
		_humans.push_back( new Human );
		glm::vec2 position( randX( randomEngine ) * TILE_WIDTH, randY( randomEngine ) * TILE_WIDTH );
		_humans.back()->init( HUMAN_SPEED, position );
	}

	// Add the zombies
	const std::vector<glm::vec2>& zombiesPositions = _level->getZombiesPositions();

	for ( int i = 0; i < zombiesPositions.size(); i++ )
	{
		_zombies.push_back( new Zombie );
		_zombies.back()->init( ZOMBIE_SPEED, zombiesPositions[i] );
	}

	// Set up the player guns
	_player->addGun( new Gun( "Magnum", 10, 1, 5.0f, 30, BULLET_SPEED ) );
	_player->addGun( new Gun( "Shotgun", 30, 12, 20.0f, 4, BULLET_SPEED ) );
	_player->addGun( new Gun( "MP5", 2, 1, 10.0f, 20, BULLET_SPEED ) );
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
	Bengine::FPSLimiter fpsLimiter;
	fpsLimiter.setMaxFPS(60.0f);

	while (_gameState == GameState::PLAY)
	{
		fpsLimiter.begin();

		checkVictory();

		processInput();

		updateAgents();
		updateBullets();

		_camera.setPosition( _player->getPosition() );
		_camera.update();

		drawGame();

		_fps = fpsLimiter.end();
	}
}

void MainGame::updateAgents()
{
	// Update all humans
	std::for_each( _humans.begin(), _humans.end(), [&]( Human* h ) { h->update( _level->getLevelData(), _humans, _zombies ); } );

	// Update Zombies
	std::for_each( _zombies.begin(), _zombies.end(), [&]( Zombie* z ) { z->update( _level->getLevelData(), _humans, _zombies ); } );
	

	// Update Zombie collisions
	for ( int i = 0; i < _zombies.size(); i++ )
	{
		// Collide with other zombies
		for ( int j = i + 1; j < _zombies.size(); j++ )
		{
			_zombies[ i ]->collideWithAgent( _zombies[ j ] );
		}

		// Collide with humans
		for ( int j = 1; j < _humans.size(); j++ )
		{
			if ( _zombies[ i ]->collideWithAgent( _humans[ j ] ) )
			{
				// Add the new zombie
				_zombies.push_back( new Zombie );
				_zombies.back()->init( ZOMBIE_SPEED, _humans[ j ]->getPosition() );

				// Delete the human
				delete _humans[ j ];
				_humans[ j ] = _humans.back();
				_humans.pop_back();
			}
		}

		// Check collision with player
		if ( _zombies[ i ]->collideWithAgent( _player ) )
		{
			Bengine::fatalError( "GAME OVER" );
		}
	}

	// Update Human collisions
	for ( int i = 0; i < _humans.size(); i++ )
	{
		// Collide with other humans
		for ( int j = i + 1; j < _humans.size(); j++ )
		{
			_humans[ i ]->collideWithAgent( _humans[ j ] );
		}
	}

}

void MainGame::updateBullets()
{
	// Update and collide with world
	for ( int i = 0; i < _bullets.size(); )
	{
		// If update returns true, the bullet collided with a wall
		if ( _bullets[ i ].update( _level->getLevelData() ) )
		{
			// Remove the bullet
			_bullets[ i ] = _bullets.back();
			_bullets.pop_back();
		}
		else
		{
			i++;
		}
	}
	
	bool wasBulletRemoved;

	// Bullet collision with zombies and humans
	for ( int b = 0; b < _bullets.size(); b++ )
	{
		wasBulletRemoved = false;

		// Loop through zombies
		for ( int z = 0; z < _zombies.size(); )
		{
			// Check collision
			if ( _bullets[ b ].collideWithAgent( _zombies[ z ] ) )
			{
				// Damage zombie, and kill it if its out of health
				if ( _zombies[ z ]->applyDamage( _bullets[ b ].getDamage() ) )
				{
					// If the zombie died, remove it
					delete _zombies[ z ];
					_zombies[ z ] = _zombies.back();
					_zombies.pop_back();
					_numZombiesKilled++;
				}
				else
				{
					z++;
				}

				// Remove the bullet
				_bullets[ b ] = _bullets.back();
				_bullets.pop_back();

				wasBulletRemoved = true;
				break;
			}
			else
			{
				// Next zombie
				z++;
			}
		}

		if ( !wasBulletRemoved )
		{
			// Loop through humans
			for ( int h = 1; h < _humans.size(); )
			{
				// Check collision
				if ( _bullets[ b ].collideWithAgent( _humans[ h ] ) )
				{
					// Damage human, and kill it if its out of health
					if ( _humans[ h ]->applyDamage( _bullets[ b ].getDamage() ) )
					{
						// If the human died, remove it
						delete _humans[ h ];
						_humans[ h ] = _humans.back();
						_humans.pop_back();
						_numHumansKilled++;
					}
					else
					{
						h++;
					}

					// Remove the bullet
					_bullets[ b ] = _bullets.back();
					_bullets.pop_back();

					wasBulletRemoved = true;
					break;
				}
				else
				{
					// Next human
					h++;
				}
			}
		}
	}
}

void MainGame::checkVictory()
{
	// TODO: Support for multipel levels - _currentLevel++, initLevel()

	// If all zombies are dead, we win
	if ( _zombies.empty() )
	{
		std::printf( "*** You killed %d humans and %d zombies.\n*** There are %d (of %d) humans remaining.\n",
			_numHumansKilled, _numZombiesKilled, _humans.size() - 1, _level->getNumHumans() );
		Bengine::fatalError( "You Win!" );
	}
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
			_gameState = GameState::EXIT;
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

	_textureProgram.use();

	// Draw code goes here
	glActiveTexture(GL_TEXTURE0);

	// Make sure the shader uses texture 0
	GLint textureUniform = _textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// Grab the camera matrix
	glm::mat4 projectionMatrix = _camera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	// Draw level
	_level->draw();

	// Begin drawing agents
	_agentSpriteBatch.begin();

	// Draw all humans & zombies
	std::for_each( _humans.begin(), _humans.end(), [&]( Human* h ) { h->draw( _agentSpriteBatch ); } );
	std::for_each( _zombies.begin(), _zombies.end(), [&]( Zombie* z ) { z->draw( _agentSpriteBatch ); } );

	// Draw the bullets
	std::for_each( _bullets.begin(), _bullets.end(), [&]( Bullet b ) { b.draw( _agentSpriteBatch ); } );

	//for ( int i = 0; i < _bullets.size(); i++ )
	//{
	//	_bullets[ i ].draw( _agentSpriteBatch );
	//}

	_agentSpriteBatch.end();
	_agentSpriteBatch.render();

	_textureProgram.unuse();

	// Swap our buffer and draw everything to the screen
	_window.swapBuffer();
}
