#include "MainGame.h"

#include <SDL/SDL.h>

#include "Zombie.h"
#include "Gun.h"

#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

MainGame::MainGame()
{
	// Empty
}

MainGame::~MainGame()
{
	delete m_level;

	// Delete all humans remaining
	std::for_each( m_humans.begin(), m_humans.end(), [&]( Human* h ) { delete h; } );

	// Delete all zombies remaining
	std::for_each( m_zombies.begin(), m_zombies.end(), [&]( Zombie* z ) { delete z; } );
}

/// Runs the game
void MainGame::run()
{
	initSystems();
	initLevel();
	initPlayer();

	Bengine::Music music = m_audioEngine.loadMusic( "Sound/XYZ.ogg" );
	music.play( -1 );

	gameLoop();
}

/// Initializes the core systems
void MainGame::initSystems()
{
	Bengine::init();

	// Initialize sound must happen after Bengine::Init
	m_audioEngine.init();

	m_window.create("Zombie Game", m_screenWidth, m_screenHeight, 0);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	initShaders();

	// Initialize our spritebatches
	m_agentSpriteBatch.init();
	m_hudSpriteBatch.init();

	// Initialize sprite font
	m_spriteFont = new Bengine::SpriteFont( "Fonts/chintzy.ttf", 64 );

	// Set up the camera
	m_camera.init( m_screenWidth, m_screenHeight );
	m_hudCamera.init(m_screenWidth, m_screenHeight);
	std::cout << "width " << m_screenWidth << " height " << m_screenHeight << std::endl;
	//_hudCamera.setPosition( glm::vec2( 0, 150 ) );
	m_hudCamera.setPosition( glm::vec2( m_screenWidth / 5, m_screenHeight / 2 ) );

	// Initialize particles
	m_bloodParticleBatch = new Bengine::ParticleBatch2D();
	m_bloodParticleBatch->init( 1000, 0.05f, Bengine::ResourceManager::getTexture( "Textures/particle.png" ));
	m_particleEngine.addParticleBatch( m_bloodParticleBatch );
}

void MainGame::initLevel()
{
	// Level 1
	m_currentLevel = 1;
	m_level = new Level("Levels/level1.txt");

	std::cout << "Current level " << m_currentLevel << std::endl;
}

void MainGame::initPlayer()
{
	m_player = new Player();
	m_player->init( PLAYER_SPEED, m_level->getPlayerPosition(), &m_inputManager, &m_camera, &m_bullets);

	m_humans.push_back(m_player);

	std::cout << m_level->getWidth() - 1 << "\n" << m_level->getHeight() - 1 << std::endl;

	std::mt19937 randomEngine;
	randomEngine.seed( time( nullptr ) );
	std::uniform_int_distribution<int> randX( 2, m_level->getWidth() - 2 );
	std::uniform_int_distribution<int> randY( 2, m_level->getHeight() - 2 );

	// Add humans at random positions
	for ( int i = 0; i < m_level->getNumHumans(); i++ )
	{
		m_humans.push_back( new Human );
		glm::vec2 position( randX( randomEngine ) * TILE_WIDTH, randY( randomEngine ) * TILE_WIDTH );
		m_humans.back()->init( HUMAN_SPEED, position );
	}

	// Add the zombies
	const std::vector<glm::vec2>& zombiesPositions = m_level->getZombiesPositions();

	for ( int i = 0; i < zombiesPositions.size(); i++ )
	{
		m_zombies.push_back( new Zombie );
		m_zombies.back()->init( ZOMBIE_SPEED, zombiesPositions[i] );
	}

	// Set up the player guns
	m_player->addGun( new Gun( "Magnum", 10, 1, 5.0f, 30, BULLET_SPEED, m_audioEngine.loadSoundEffect( "Sound/shots/pistol.wav" ) ) );
	m_player->addGun( new Gun( "Shotgun", 30, 12, 20.0f, 4, BULLET_SPEED, m_audioEngine.loadSoundEffect( "Sound/shots/shotgun.wav" ) ) );
	m_player->addGun( new Gun( "MP5", 2, 1, 10.0f, 20, BULLET_SPEED, m_audioEngine.loadSoundEffect( "Sound/shots/cg1.wav" ) ) );
}

/// Initializes the shaders
void MainGame::initShaders()
{
	// Compile our color shader
	m_textureProgram.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();
}

/// Main game loop for the program
void MainGame::gameLoop()
{
	const float DESIRED_FPS = 60.0f;
	const int MAX_PHYSICS_STEPS = 6;

	Bengine::FPSLimiter fpsLimiter;
	fpsLimiter.setMaxFPS( DESIRED_FPS );

	// Zoom out the camera
	const float CAMERA_SCALE = 1.0f / 2.5f;
	m_camera.setScale( CAMERA_SCALE );

	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;

	float previousTicks = SDL_GetTicks();

	while (m_gameState == GameState::PLAY)
	{
		fpsLimiter.begin();

		float newTicks = SDL_GetTicks();
		float frameTime = newTicks - previousTicks;
		previousTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		checkVictory();

		m_inputManager.update();

		processInput();

		int i = 0;

		while ( totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS )
		{
			float deltaTime = std::min( totalDeltaTime, MAX_DELTA_TIME );
			updateAgents( deltaTime );
			updateBullets( deltaTime );
			m_particleEngine.update( deltaTime );
			totalDeltaTime -= deltaTime;

			i++;
		}


		m_camera.setPosition( m_player->getPosition() );
		m_camera.update();
		m_hudCamera.update();

		drawGame();

		m_fps = fpsLimiter.end();
	}
}

void MainGame::updateAgents( float deltaTime )
{
	// Update all humans
	std::for_each( m_humans.begin(), m_humans.end(), [&]( Human* h ) { h->update( m_level->getLevelData(), m_humans, m_zombies, deltaTime ); } );

	// Update Zombies
	std::for_each( m_zombies.begin(), m_zombies.end(), [&]( Zombie* z ) { z->update( m_level->getLevelData(), m_humans, m_zombies, deltaTime ); } );
	

	// Update Zombie collisions
	for ( int i = 0; i < m_zombies.size(); i++ )
	{
		// Collide with other zombies
		for ( int j = i + 1; j < m_zombies.size(); j++ )
		{
			m_zombies[ i ]->collideWithAgent( m_zombies[ j ] );
		}

		// Collide with humans
		for ( int j = 1; j < m_humans.size(); j++ )
		{
			if ( m_zombies[ i ]->collideWithAgent( m_humans[ j ] ) )
			{
				// Add the new zombie
				m_zombies.push_back( new Zombie );
				m_zombies.back()->init( ZOMBIE_SPEED, m_humans[ j ]->getPosition() );

				// Delete the human
				delete m_humans[ j ];
				m_humans[ j ] = m_humans.back();
				m_humans.pop_back();
			}
		}

		// Check collision with player
		if ( m_zombies[ i ]->collideWithAgent( m_player ) )
		{
			Bengine::fatalError( "GAME OVER" );
		}
	}

	// Update Human collisions
	for ( int i = 0; i < m_humans.size(); i++ )
	{
		// Collide with other humans
		for ( int j = i + 1; j < m_humans.size(); j++ )
		{
			m_humans[ i ]->collideWithAgent( m_humans[ j ] );
		}
	}

}

void MainGame::updateBullets( float deltaTime )
{
	// Update and collide with world
	for ( int i = 0; i < m_bullets.size(); )
	{
		// If update returns true, the bullet collided with a wall
		if ( m_bullets[ i ].update( m_level->getLevelData(), deltaTime ) )
		{
			// Remove the bullet
			m_bullets[ i ] = m_bullets.back();
			m_bullets.pop_back();
		}
		else
		{
			i++;
		}
	}
	
	bool wasBulletRemoved;

	// Bullet collision with zombies and humans
	for ( int b = 0; b < m_bullets.size(); b++ )
	{
		wasBulletRemoved = false;

		// Loop through zombies
		for ( int z = 0; z < m_zombies.size(); )
		{
			// Check collision
			if ( m_bullets[ b ].collideWithAgent( m_zombies[ z ] ) )
			{
				// Add blood
				addBlood( m_bullets[ b ].getPosition(), 5 );

				// Damage zombie, and kill it if its out of health
				if ( m_zombies[ z ]->applyDamage( m_bullets[ b ].getDamage() ) )
				{
					// If the zombie died, remove it
					delete m_zombies[ z ];
					m_zombies[ z ] = m_zombies.back();
					m_zombies.pop_back();
					m_numZombiesKilled++;
				}
				else
				{
					z++;
				}

				// Remove the bullet
				m_bullets[ b ] = m_bullets.back();
				m_bullets.pop_back();

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
			for ( int h = 1; h < m_humans.size(); )
			{
				// Check collision
				if ( m_bullets[ b ].collideWithAgent( m_humans[ h ] ) )
				{
					// Add blood
					addBlood( m_bullets[ b ].getPosition(), 5 );

					// Damage human, and kill it if its out of health
					if ( m_humans[ h ]->applyDamage( m_bullets[ b ].getDamage() ) )
					{
						// If the human died, remove it
						delete m_humans[ h ];
						m_humans[ h ] = m_humans.back();
						m_humans.pop_back();
						m_numHumansKilled++;
					}
					else
					{
						h++;
					}

					// Remove the bullet
					m_bullets[ b ] = m_bullets.back();
					m_bullets.pop_back();

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
	if ( m_zombies.empty() )
	{
		std::printf( "*** You killed %d humans and %d zombies.\n*** There are %d (of %d) humans remaining.\n",
			m_numHumansKilled, m_numZombiesKilled, m_humans.size() - 1, m_level->getNumHumans() );
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
			m_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			m_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
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

	m_textureProgram.use();

	// Draw code goes here
	glActiveTexture(GL_TEXTURE0);

	// Make sure the shader uses texture 0
	GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// Grab the camera matrix
	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
	GLint pUniform = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	// Draw level
	m_level->draw();

	// Begin drawing agents
	m_agentSpriteBatch.begin();

	const glm::vec2 agentDims( AGENT_RADIUS * 2.0f );

	// Draw all humans & zombies
	std::for_each( m_humans.begin(), m_humans.end(), [&]( Human* h )
		{
			if ( m_camera.isBoxInView(h->getPosition(), agentDims ) )
			{
				h->draw( m_agentSpriteBatch );
			}		
		} );
	std::for_each( m_zombies.begin(), m_zombies.end(), [&]( Zombie* z )
		{
			if ( m_camera.isBoxInView( z->getPosition(), agentDims ) )
			{
				z->draw( m_agentSpriteBatch );
			}
		} );

	// Draw the bullets
	std::for_each( m_bullets.begin(), m_bullets.end(), [&]( Bullet b ) { b.draw( m_agentSpriteBatch ); } );

	//for ( int i = 0; i < m_bullets.size(); i++ )
	//{
	//	m_bullets[ i ].draw( _agentSpriteBatch );
	//}

	m_agentSpriteBatch.end();
	m_agentSpriteBatch.render();

	// Render the particles
	m_particleEngine.draw( &m_agentSpriteBatch );

	// Render the heads up display
	drawHud();

	m_textureProgram.unuse();

	// Swap our buffer and draw everything to the screen
	m_window.swapBuffer();
}

void MainGame::drawHud()
{
	char buffer[ 256 ];

	glm::mat4 projectionMatrix = m_hudCamera.getCameraMatrix();
	GLint pUniform = m_textureProgram.getUniformLocation( "P" );
	glUniformMatrix4fv( pUniform, 1, GL_FALSE, &projectionMatrix[ 0 ][ 0 ] );

	m_hudSpriteBatch.begin();

	sprintf_s( buffer, "Num Humans %d", m_humans.size() );

	m_spriteFont->draw( 
		m_hudSpriteBatch, 
		buffer, 
		glm::vec2( 0, 0 ), 
		glm::vec2( 0.5f ), 
		0.0f,
		Bengine::ColorRGBA8( 255, 255, 255, 255 ), 
		Bengine::Justification::RIGHT
	);

	sprintf_s( buffer, "Num Zombies %d", m_zombies.size() );

	m_spriteFont->draw(
		m_hudSpriteBatch,
		buffer,
		glm::vec2( 0, 36 ),
		glm::vec2( 0.5f ),								/// Scaling of text
		0.0f,
		Bengine::ColorRGBA8( 255, 255, 255, 255 ),
		Bengine::Justification::RIGHT
	);
	m_hudSpriteBatch.end();
	m_hudSpriteBatch.render();
}

void MainGame::addBlood( const glm::vec2& position, int numParticles )
{
	static std::mt19937 randEngine( time( NULL ) );
	static std::uniform_real_distribution<float> randAngle( 0.0f, 360.0f );

	glm::vec2 velocity( 2.0f, 0.0f );
	Bengine::ColorRGBA8 color( 255, 0, 0, 255 );

	for ( int i = 0; i < numParticles; i++ )
	{
		m_bloodParticleBatch->addParticle( position, glm::rotate( velocity, randAngle( randEngine ) ), color, 30.0f );
	}
}
