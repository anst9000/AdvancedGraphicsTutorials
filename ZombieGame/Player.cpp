#include <SDL/SDL.h>

#include "Player.h"
#include "Gun.h"
#include <Bengine/ResourceManager.h>


Player::Player()
{
	// Empty
}

Player::~Player()
{
	// Empty
}

void Player::init(
	float speed, 
	glm::vec2 position, 
	Bengine::InputManager* inputManager, 
	Bengine::Camera2D* camera,
	std::vector<Bullet>* bullets
)
{
	m_speed = speed;
	m_position = position;
	m_inputManager = inputManager;
	m_camera = camera;
	m_bullets = bullets;

	m_health = 150.0f;

	m_color = Bengine::ColorRGBA8( 255, 255, 255, 255 );

	m_textureID = Bengine::ResourceManager::getTexture( "Textures/player.png" ).id;
}

void Player::addGun( Gun* gun )
{
	// Add the gun to players inventory
	m_guns.push_back( gun );

	// If no gun equipped, equip gun
	if ( m_currentGunIndex == -1 )
	{
		m_currentGunIndex = 0;
	}
}

void Player::update(
	const std::vector<std::string>& levelData,
	std::vector<Human*>& humans,
	std::vector<Zombie*>& zombies,
	float deltaTime
)
{
	if ( m_inputManager->isKeyDown( SDLK_w ) )
	{
		m_position.y += m_speed * deltaTime;
	}
	else if ( m_inputManager->isKeyDown( SDLK_s ) )
	{
		m_position.y -= m_speed * deltaTime;
	}
	else if ( m_inputManager->isKeyDown( SDLK_a ) )
	{
		m_position.x -= m_speed * deltaTime;
	}
	else if ( m_inputManager->isKeyDown( SDLK_d ) )
	{
		m_position.x += m_speed * deltaTime;
	}

	// Switch gun
	if ( m_inputManager->isKeyDown( SDLK_1 ) )
	{
		m_currentGunIndex = 0;
	}
	else if ( m_inputManager->isKeyDown( SDLK_2 ) )
	{
		m_currentGunIndex = 1;
	}
	else if ( m_inputManager->isKeyDown( SDLK_3 ) )
	{
		m_currentGunIndex = 2;
	}

	glm::vec2 mouseCoords = m_inputManager->getMouseCoords();
	mouseCoords = m_camera->convertScreenToWorld( mouseCoords );

	glm::vec2 centerPosition = m_position + glm::vec2( AGENT_RADIUS );
	m_direction = glm::normalize( mouseCoords - centerPosition );

	if ( m_currentGunIndex != -1 )
	{
		m_guns[ m_currentGunIndex ]->update(
			m_inputManager->isKeyDown( SDL_BUTTON_LEFT ),
			centerPosition,
			m_direction,
			*m_bullets,
			deltaTime
		);
	}

	collideWithLevel( levelData );
}
