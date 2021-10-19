#pragma once

#include <Bengine/InputManager.h>
#include <Bengine/Camera2D.h>

#include "Human.h"
#include "Bullet.h"

const float PLAYER_SPEED = 6.0f;

class Gun;

class Player : public Human
{
public:
	Player();
	~Player();
	
	void init(
		float speed, 
		glm::vec2 position, 
		Bengine::InputManager* inputManager, 
		Bengine::Camera2D* camera, 
		std::vector<Bullet>* bullets
	);

	void addGun( Gun* gun );

	void update(
		const std::vector<std::string>& levelData,
		std::vector<Human*>& humans,
		std::vector<Zombie*>& zombies,
		float deltaTime
	);

private:
	Bengine::InputManager* _inputManager;

	std::vector<Gun*> _guns;
	int _currentGunIndex;

	Bengine::Camera2D* _camera;
	std::vector<Bullet>* _bullets;
};

