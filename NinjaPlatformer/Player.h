#pragma once

#include <Bengine/SpriteBatch.h>
#include <Bengine/GLTexture.h>
#include <Bengine/InputManager.h>
#include "Capsule.h"

class Player
{
public:
	void init(
		b2World* world,
		const glm::vec2& position,
		const glm::vec2& drawDims,
		const glm::vec2& collisionDims,
		Bengine::ColorRGBA8 color
	);
	void draw( Bengine::SpriteBatch& spriteBatch );
	void drawDebug( Bengine::DebugRenderer& debugRenderer );

	void update( Bengine::InputManager& inputManager );

	const Capsule& getCapsule() const { return m_capsule; }

private:
	glm::vec2 m_drawDims;
	Bengine::GLTexture m_texture;
	Bengine::ColorRGBA8 m_color;
	Capsule m_capsule;
};

