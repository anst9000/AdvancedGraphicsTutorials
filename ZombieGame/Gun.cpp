#include <random>
#include <ctime>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

#include "Gun.h"

Gun::Gun( std::string name, int fireRate, int bulletsPerShot, float spread, float bulletDamage, float bulletSpeed )
	:
	_name( name ),
	_fireRate( fireRate ),
	_bulletsPerShot( bulletsPerShot ),
	_spread( spread ),
	_bulletDamage( bulletDamage ),
	_bulletSpeed( bulletSpeed ),
	_frameCounter( 0 )
{
}

Gun::~Gun()
{
}

void Gun::update( bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets )
{
	_frameCounter++;

	if ( _frameCounter >= _fireRate && isMouseDown )
	{
		fire( direction, position, bullets );
		_frameCounter = 0;
	}
}

void Gun::fire( const glm::vec2& direction, const glm::vec2& position, std::vector<Bullet>& bullets )
{
	static std::mt19937 randomEngine( time( nullptr ) );
	std::uniform_real_distribution<float> randomRotation( -_spread, _spread );

	for ( int i = 0; i < _bulletsPerShot; i++ )
	{
		bullets.emplace_back(
			position,
			glm::rotate( direction, glm::radians(randomRotation( randomEngine ) ) ),
			_bulletDamage, 
			_bulletSpeed );
	}
}
