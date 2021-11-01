#include "Camera2D.h"


namespace Bengine
{

	Camera2D::Camera2D()
		: 
		_screenWidth(500), 
		_screenHeight(500), 
		_needsMatrixUpdate(true), 
		_position(0.0f, 0.0f), 
		_cameraMatrix(1.0f),
		_orthoMatrix(1.0f),
		_scale(1.0f)
	{

	}

	Camera2D::~Camera2D()
	{
	}

	void Camera2D::init(int screenWidth, int screenHeight)
	{
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
		_orthoMatrix = glm::ortho(0.0f, (float)_screenWidth, 0.0f, (float)_screenHeight);
	} 

	void Camera2D::update()
	{
		// Only update if out position or scale have changed
		if (_needsMatrixUpdate)
		{
			// Camera translation
			glm::vec3 translate(-_position.x + _screenWidth / 2, -_position.y + _screenHeight / 2, 0.0f);
			_cameraMatrix = glm::translate(_orthoMatrix, translate);

			// Camera scale
			glm::vec3 scale(_scale, _scale, 0.0f);
			_cameraMatrix = glm::scale(glm::mat4(1.0f), scale) * _cameraMatrix;

			_needsMatrixUpdate = false;
		}
	}

	glm::vec2 Camera2D::convertScreenToWorld(glm::vec2 screenCoords)
	{
		// Invert y-direction
		screenCoords.y = _screenHeight - screenCoords.y;

		// Make origo the center
		screenCoords -= glm::vec2(_screenWidth / 2, _screenHeight / 2);
		// Scale the coordinates
		screenCoords /= _scale;
		// Translate with the camera position
		screenCoords += _position;

		return screenCoords;
	}

	// Simple AABB test to see if a box is in the camera view
	bool Camera2D::isBoxInView( const glm::vec2& position, const glm::vec2& dimensions )
	{
		glm::vec2 scaledScreenDimensions = glm::vec2( _screenWidth, _screenHeight ) / _scale;

		const float MIN_DISTANCE_X = dimensions.x / 2.0f + scaledScreenDimensions.x / 2.0f;
		const float MIN_DISTANCE_Y = dimensions.y / 2.0f + scaledScreenDimensions.y / 2.0f;

		// Center position of the parameters
		glm::vec2 centerPos = position + dimensions / 2.0f;
		// Center position of the camera
		glm::vec2 centerCameraPos = _position;
		// Vector from the input to the camera
		glm::vec2 distVec = centerPos - centerCameraPos;

		float xDepth = MIN_DISTANCE_X - std::abs( distVec.x );
		float yDepth = MIN_DISTANCE_Y - std::abs( distVec.y );

		if ( xDepth > 0.0f && yDepth > 0.0f )
		{
			// There was a collision
			return true;
		}

		return false;
	}

}