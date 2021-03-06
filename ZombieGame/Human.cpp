#include "Human.h"

#include <random>
#include <ctime>
#include <glm/gtx/rotate_vector.hpp>
#include <Bengine/ResourceManager.h>

Human::Human()
{}

Human::~Human()
{}

void Human::init( float speed, glm::vec2 position )
{
	static std::mt19937 randomEngine( (unsigned int)time( nullptr ) );
	static std::uniform_real_distribution<float> randomDirection( -1.0f, 1.0f );

	m_health = 20.0f;

	m_color = Bengine::ColorRGBA8( 255, 255, 255, 100 );

	m_speed = speed;
	m_position = position;

	// Get random direction
	m_direction = glm::vec2( randomDirection( randomEngine ), randomDirection( randomEngine ) );

	// Make sure direction is not zero
	if ( m_direction.length() == 0 )
	{
		m_direction = glm::vec2( 1.0f, 0.0f );
	}

	m_direction = glm::normalize( m_direction );

	m_textureID = Bengine::ResourceManager::getTexture( "Textures/human.png" ).id;
}

void Human::update(
	const std::vector<std::string>& levelData,
	std::vector<Human*>& humans,
	std::vector<Zombie*>& zombies,
	float deltaTime
)
{
	static std::mt19937 randomEngine( (unsigned int)time( nullptr ) );
	static std::uniform_real_distribution<float> randomRotation( -40.0f, 40.0f );

	m_position += m_direction * m_speed * deltaTime;

	if ( m_frames == 100 )
	{
		m_direction = glm::rotate( m_direction, randomRotation( randomEngine ) );
		m_frames = 0;
	}
	else
	{
		m_frames++;
	}

	if ( collideWithLevel( levelData ) )
	{
		m_direction = glm::rotate( m_direction, randomRotation( randomEngine ) );
	}
}
