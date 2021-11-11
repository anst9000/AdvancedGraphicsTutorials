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
	m_texture = Bengine::ResourceManager::getTexture( "Assets/blue_ninja.png" );
	m_drawDims = drawDims;
	m_color = color;
	m_capsule.init( world, position, collisionDims, 1.0f, 0.1f, true );
}

void Player::draw( Bengine::SpriteBatch& spriteBatch )
{
	glm::vec4 destRect;
	b2Body* body = m_capsule.getBody();
	destRect.x = body->GetPosition().x - m_drawDims.x / 2.0f;
	destRect.y = body->GetPosition().y - m_capsule.getDimensions().y / 2.0f;
	destRect.z = m_drawDims.x;
	destRect.w = m_drawDims.y;

	spriteBatch.draw( destRect, glm::vec4( 0.0f, 0.0f, 0.1f, 0.5f ), m_texture.id, 0.0f, m_color, body->GetAngle() );
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
	}
	else if ( inputManager.isKeyDown( SDLK_d ) )
	{
		body->ApplyForceToCenter( b2Vec2( 100.0f, 0.0f ), true );
	}
	else
	{
		body->SetLinearVelocity( b2Vec2( body->GetLinearVelocity().x * 0.95, body->GetLinearVelocity().y ) );
	}


	float MAX_SPEED = 4.5f;
	if ( body->GetLinearVelocity().x < -MAX_SPEED )
	{
		body->SetLinearVelocity( b2Vec2( -MAX_SPEED, body->GetLinearVelocity().y ) );
	}
	else if ( body->GetLinearVelocity().x > MAX_SPEED )
	{
		body->SetLinearVelocity( b2Vec2( MAX_SPEED, body->GetLinearVelocity().y ) );
	}

	// Loop through all the contact points
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
				if ( manifold.points[ i ].y < body->GetPosition().y - m_capsule.getDimensions().y / 2.0f + 0.01f )
				{
					below = true;
					break;
				}
			}

			if ( below )
			{
				// We can jump
				if ( inputManager.isKeyPressed( SDLK_w ) )
				{
					body->ApplyLinearImpulse( b2Vec2( 0.0f, 15.0f ), b2Vec2( 0.0f, 0.0f ), true );
					break;
				}
			}
		}
	}

	// Apply damping

}

