#include "Human.h"

#include <random>
#include <ctime>
#include <glm/gtx/rotate_vector.hpp>

Human::Human() : _frames(0)
{}

Human::~Human()
{}

void Human::init( float speed, glm::vec2 position )
{
	static std::mt19937 randomEngine( time( nullptr ) );
	static std::uniform_real_distribution<float> randomDirection( -1.0f, 1.0f );

	_health = 20.0f;

	_color = Bengine::ColorRGBA8( 200, 0, 200, 255 );

	_speed = speed;
	_position = position;

	// Get random direction
	_direction = glm::vec2( randomDirection( randomEngine ), randomDirection( randomEngine ) );

	// Make sure direction is not zero
	if ( _direction.length() == 0 )
	{
		_direction = glm::vec2( 1.0f, 0.0f );
	}

	_direction = glm::normalize( _direction );
}

void Human::update(
	const std::vector<std::string>& levelData,
	std::vector<Human*>& humans,
	std::vector<Zombie*>& zombies,
	float deltaTime
)
{
	static std::mt19937 randomEngine( time( nullptr ) );
	static std::uniform_real_distribution<float> randomRotation( -40.0f, 40.0f );

	_position += _direction * _speed * deltaTime;

	if ( _frames == 50 )
	{
		_frames = 0;
	}
	else
	{
		_frames++;
	}

	if ( collideWithLevel( levelData ) )
	{
		_direction = glm::rotate( _direction, randomRotation( randomEngine ) );
	}
}
