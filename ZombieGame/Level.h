#pragma once

#include <string>
#include <vector>
#include <map>

#include <Bengine/SpriteBatch.h>

const int TILE_WIDTH = 64;
const float TILE_RADIUS = (float)TILE_WIDTH / 2.0f;

class Level
{
public:
	// Load the level
	Level(const std::string& filename);
	~Level();

	void printLevel();
	void draw();

	// Getters
	int getWidth() const { return _levelData[0].size(); }
	int getHeight() const { return _levelData.size(); }
	int getNumHumans() const { return _numHumans; }

	const std::vector<std::string>& getLevelData() const { return _levelData; }
	glm::vec2 getPlayerPosition() const { return _playerPosition; }
	const std::vector<glm::vec2>& getZombiesPositions() const { return _zombiesPositions; }

private:
	std::vector<std::string> _levelData;
	int _numHumans;

	Bengine::SpriteBatch _spriteBatch;

	glm::vec2 _playerPosition;
	std::vector<glm::vec2> _zombiesPositions;
};

