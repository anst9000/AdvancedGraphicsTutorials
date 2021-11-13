#include <SDL/SDL.h>
#include <iostream>

#include "Player.h"

#include <Bengine/ResourceManager.h>

void Player::init(
	b2World* world,
	const glm::vec2& position,
	const glm::vec2& drawDims,
	const glm::vec2& collisionDims,
	Bengine::ColorRGBA8 color
)
{
	Bengine::GLTexture texture = Bengine::ResourceManager::getTexture( "Assets/blue_ninja.png" );
	m_drawDims = drawDims;
	m_color = color;
	m_capsule.init( world, position, collisionDims, 1.0f, 0.1f, true );
	m_texture.init( texture, glm::ivec2( 10, 2 ) );
}

void Player::draw( Bengine::SpriteBatch& spriteBatch )
{
	glm::vec4 destRect;
	b2Body* body = m_capsule.getBody();
	destRect.x = body->GetPosition().x - m_drawDims.x / 2.0f;
	destRect.y = body->GetPosition().y - m_capsule.getDimensions().y / 2.0f;
	destRect.z = m_drawDims.x;
	destRect.w = m_drawDims.y;

	int tileIndex;
	int numTiles;

	float animationSpeed = 0.2f;
	glm::vec2 velocity;
	velocity.x = body->GetLinearVelocity().x;
	velocity.y = body->GetLinearVelocity().y;

	// Calculate animation
	if ( m_onGround )
	{
		if ( m_isPunching )
		{
			numTiles = 4;
			tileIndex = 1;

			if ( m_moveState != PlayerMoveState::PUNCHING )
			{
				m_moveState = PlayerMoveState::PUNCHING;
				m_animationTime = 0.0f;
			}
		}
		else if ( abs( velocity.x ) > 1.0f && ( ( velocity.x > 0.0f && !m_moveLeft ) || ( velocity.x < 0.0f && m_moveLeft ) ) )
		{
			// Running
			numTiles = 6;
			tileIndex = 10;
			animationSpeed = abs( velocity.x ) * 0.050f;

			if ( m_moveState != PlayerMoveState::RUNNING )
			{
				m_moveState = PlayerMoveState::RUNNING;
				m_animationTime = 0.0f;
			}
		}
		else
		{
			// Standing still
			numTiles = 1;
			tileIndex = 0;
			m_moveState = PlayerMoveState::STANDING;
		}
	}
	else
	{
		// In the air
		if ( m_isPunching )
		{
			numTiles = 1;
			tileIndex = 18;
			animationSpeed *= 0.25f;

			if ( m_moveState != PlayerMoveState::PUNCHING )
			{
				m_moveState = PlayerMoveState::PUNCHING;
				m_animationTime = 0.0f;
			}
		}
		else if ( abs( velocity.x ) > 10.0f )
		{
			numTiles = 1;
			tileIndex = 10;
		} 
		else if ( velocity.y <= 0.0f )
		{
			// Falling
			numTiles = 1;
			tileIndex = 17;
		}
		else
		{
			// Rising
			numTiles = 1;
			tileIndex = 16;
		}

		m_moveState = PlayerMoveState::IN_AIR;
	}

	// Increment animation time
	m_animationTime += animationSpeed;

	// Check for punch end
	if ( m_animationTime > numTiles )
	{
		m_isPunching = false;
	}

	// Apply animation
	tileIndex += (int)m_animationTime % numTiles;

	// Get the uv coordinates from the tile index
	glm::vec4 uvRect = m_texture.getUVs( tileIndex );

	// Check direction
	if ( m_moveLeft )
	{
		uvRect.x += 1.0f / m_texture.dims.x;
		uvRect.z *= -1.0f;
	}

	// Draw the sprite
	spriteBatch.draw( destRect, uvRect, m_texture.texture.id, 0.0f, m_color, body->GetAngle() );
}

void Player::drawDebug( Bengine::DebugRenderer& debugRenderer )
{
	m_capsule.drawDebug( debugRenderer );
}

void Player::update( Bengine::InputManager& inputManager )
{
	b2Body* body = m_capsule.getBody();

	if ( inputManager.isKeyDown( SDLK_a ) )
	{
		body->ApplyForceToCenter( b2Vec2( -100.0f, 0.0f ), true );
		m_moveLeft = true;
	}
	else if ( inputManager.isKeyDown( SDLK_d ) )
	{
		body->ApplyForceToCenter( b2Vec2( 100.0f, 0.0f ), true );
		m_moveLeft = false;
	}
	else
	{
		body->SetLinearVelocity( b2Vec2( body->GetLinearVelocity().x * 0.95, body->GetLinearVelocity().y ) );
	}

	if ( inputManager.isKeyPressed( SDLK_SPACE ) )
	{
		// Check for punch or kick
		m_isPunching = true;
	}

	float MAX_SPEED = 10.0f;
	if ( body->GetLinearVelocity().x < -MAX_SPEED )
	{
		body->SetLinearVelocity( b2Vec2( -MAX_SPEED, body->GetLinearVelocity().y ) );
	}
	else if ( body->GetLinearVelocity().x > MAX_SPEED )
	{
		body->SetLinearVelocity( b2Vec2( MAX_SPEED, body->GetLinearVelocity().y ) );
	}

	// Loop through all the contact points
	m_onGround = false;

	for ( b2ContactEdge* contactEdge = body->GetContactList(); contactEdge != nullptr; contactEdge = contactEdge->next )
	{
		b2Contact* contact = contactEdge->contact;
		if ( contact->IsTouching() )
		{
			b2WorldManifold manifold;
			contact->GetWorldManifold( &manifold );

			// Check if the points are below
			bool below = false;
			for ( int i = 0; i < b2_maxManifoldPoints; i++ )
			{
				if ( manifold.points[ i ].y < body->GetPosition().y - m_capsule.getDimensions().y / 2.0f + m_capsule.getDimensions().x / 2.0f + 0.01f )
				{
					below = true;
					break;
				}
			}

			if ( below )
			{
				m_onGround = true;

				// We can jump
				if ( inputManager.isKeyPressed( SDLK_w ) )
				{
					body->ApplyLinearImpulse( b2Vec2( 0.0f, 25.0f ), b2Vec2( 0.0f, 0.0f ), true );
					break;
				}
			}
		}
	}

	// Apply damping

}

glm::vec2 Player::getPosition() const
{
	glm::vec2 output;
	output.x = m_capsule.getBody()->GetPosition().x;
	output.y = m_capsule.getBody()->GetPosition().y;

	return output;
}

