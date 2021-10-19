
#include <Bengine/ResourceManager.h>

#include "Bullet.h"
#include "Agent.h"
#include "Human.h"
#include "Zombie.h"
#include "Level.h"

Bullet::Bullet( glm::vec2 position, glm::vec2 direction, float damage, float speed )
	:
	_position( position ),
	_direction( direction ),
	_damage( damage ),
	_speed( speed )
{
	// Empty
}

Bullet::~Bullet()
{
	// Empty
}

bool Bullet::update( const std::vector<std::string>& levelData, float deltaTime )
{
	_position += _direction * _speed * deltaTime;

	return collideWithLevel( levelData );
}

void Bullet::draw( Bengine::SpriteBatch& spriteBatch )
{
	glm::vec4 destRect( _position.x + BULLET_RADIUS, _position.y + BULLET_RADIUS, BULLET_RADIUS * 2, BULLET_RADIUS * 2 );
	const glm::vec4 uvRect( 0.0f, 0.0f, 1.0f, 1.0f );
	GLuint bulletTexture = Bengine::ResourceManager::getTexture( "Textures/circle.png" ).id;

	Bengine::ColorRGBA8 color( 75, 75, 75, 255 );

	spriteBatch.draw( destRect, uvRect, bulletTexture, 0.0f, color );
}

bool Bullet::collideWithAgent( Agent* agent )
{
	const float MIN_DISTANCE = AGENT_RADIUS + BULLET_RADIUS;

	glm::vec2 centerPosA = _position;
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2( AGENT_RADIUS );

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length( distVec );
	float collisionDepth = MIN_DISTANCE - distance;

	// We have a collision
	if ( collisionDepth > 0.0f )
	{
		return true;
	}

	return false;
}

bool Bullet::collideWithLevel( const std::vector<std::string>& levelData )
{
	glm::ivec2 gridPosition;
	gridPosition.x = floor( _position.x / (float)TILE_WIDTH );
	gridPosition.y = floor( _position.y / (float)TILE_WIDTH );

	// If we are outside the world just return
	if ( gridPosition.x < 0 || gridPosition.x > levelData[ 0 ].size() || gridPosition.y < 0 || gridPosition.y > levelData.size() )
	{
		return true;
	}

	return levelData[ gridPosition.y ][ gridPosition.x ] != '.';

	return false;
}

