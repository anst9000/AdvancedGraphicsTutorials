#include "Agent.h"
#include <Bengine/ResourceManager.h>
#include "Level.h"

#include <cmath>
#include <algorithm>

Agent::Agent()
{
}

Agent::~Agent()
{
}

bool Agent::collideWithLevel(const std::vector<std::string>& levelData)
{
	std::vector<glm::vec2> collideTilePositions;

	// Check the four corners
	// First corner
	checkTilePosition(levelData, collideTilePositions, m_position.x, m_position.y);

	// Second corner
	checkTilePosition(levelData, collideTilePositions, m_position.x + AGENT_WIDTH, m_position.y);
	
	// Third corner
	checkTilePosition(levelData, collideTilePositions, m_position.x, m_position.y + AGENT_WIDTH);
	
	// Fourth corner
	checkTilePosition(levelData, collideTilePositions, m_position.x + AGENT_WIDTH, m_position.y + AGENT_WIDTH);

	if ( collideTilePositions.empty() )
	{
		return false;
	}

	for ( auto tilePos : collideTilePositions )
	{
		collideWithTile( tilePos );
	}

	return true;
}

bool Agent::collideWithAgent( Agent* agent )
{
	const float MIN_DISTANCE = AGENT_RADIUS * 2.0f;

	glm::vec2 centerPosA = m_position + glm::vec2( AGENT_RADIUS );
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2( AGENT_RADIUS );

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length( distVec );
	float collisionDepth = MIN_DISTANCE - distance;

	// We have a collision
	if ( collisionDepth > 0.0f )
	{
		glm::vec2 collisionDepthVec = glm::normalize( distVec ) * collisionDepth;

		m_position += collisionDepthVec / 2.0f;
		agent->m_position -= collisionDepthVec / 2.0f;
		return true;
	}

	return false;
}

void Agent::draw(Bengine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRect;
	destRect.x = m_position.x;
	destRect.y = m_position.y;
	destRect.z = AGENT_WIDTH;
	destRect.w = AGENT_WIDTH;

	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	//GLuint textureID = Bengine::ResourceManager::getTexture("Textures/red_bricks.png").id;
	static int textureID = Bengine::ResourceManager::getTexture( "Textures/circle.png" ).id;

	spriteBatch.draw(destRect, uvRect, textureID, 0.0f, m_color);
}

bool Agent::applyDamage( float damage )
{
	m_health -= damage;

	return m_health <= 0.0f;
}

void Agent::checkTilePosition( const std::vector<std::string>& levelData, std::vector<glm::vec2>& collideTilePositions, float x, float y )
{
	glm::vec2 cornerPos = glm::vec2(
		std::floor( x / (float)( TILE_WIDTH ) ),
		std::floor( y / (float)( TILE_WIDTH ) ) );

	// If we are outside the world just return
	if ( cornerPos.x < 0 || cornerPos.x > levelData[0].size() || cornerPos.y < 0 || cornerPos.y > levelData.size() )
	{
		return;
	}

	if ( levelData[ cornerPos.y ][ cornerPos.x ] != '.' )
	{
		collideTilePositions.push_back( cornerPos * (float)TILE_WIDTH + glm::vec2( (float)TILE_WIDTH / 2.0f ) );
	}
}

// Access Aligned Bounding Box - Collision (= AABB)
void Agent::collideWithTile( glm::vec2 tilePos )
{
	const float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;

	glm::vec2 centerPlayerPos = m_position + glm::vec2( AGENT_RADIUS );
	glm::vec2 distVec = centerPlayerPos - tilePos;

	float xDepth = MIN_DISTANCE - std::abs( distVec.x );
	float yDepth = MIN_DISTANCE - std::abs( distVec.y );

	if ( xDepth > 0.0f && yDepth > 0.0f )
	{
		if ( std::max( xDepth, 0.0f ) < std::max( yDepth, 0.0f ) )
		{
			if ( distVec.x < 0.0f )
			{
				m_position.x -= xDepth;
			}
			else
			{
				m_position.x += xDepth;
			}
		}
		else
		{
			if ( distVec.y < 0.0f )
			{
				m_position.y -= yDepth;
			}
			else
			{
				m_position.y += yDepth;
			}
		}
	}
}
