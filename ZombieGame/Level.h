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
	int getWidth() const { return m_levelData[0].size(); }
	int getHeight() const { return m_levelData.size(); }
	int getNumHumans() const { return m_numHumans; }

	const std::vector<std::string>& getLevelData() const { return m_levelData; }
	glm::vec2 getPlayerPosition() const { return m_playerPosition; }
	const std::vector<glm::vec2>& getZombiesPositions() const { return m_zombiesPositions; }

private:
	std::vector<std::string> m_levelData;
	int m_numHumans = 0;

	Bengine::SpriteBatch m_spriteBatch;

	glm::vec2 m_playerPosition;
	std::vector<glm::vec2> m_zombiesPositions;
};

