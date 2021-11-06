#include "Level.h"
#include <Bengine/BengineErrors.h>
#include <Bengine/ResourceManager.h>

#include <fstream>
#include <iostream>
#include <algorithm>

Level::Level(const std::string& fileName)
{
	std::ifstream readIn;
	readIn.open(fileName);

	// Error checking
	if (readIn.fail())
	{
		Bengine::fatalError("Failed to open " + fileName);
	}

	// Throw away the first string in level to tmp
	std::string tmp;
	readIn >> tmp >> m_numHumans;

	std::string line;
	std::getline( readIn, line );

	while (std::getline(readIn, line))
	{
		m_levelData.push_back(line);
	}

	//// Print the level
	//printLevel();

	m_spriteBatch.init();
	m_spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	Bengine::ColorRGBA8 whiteColor( 255, 255, 255, 255 );
	
	GLuint redBricks = Bengine::ResourceManager::getTexture("Textures/red_bricks.png").id;
	GLuint lightBricks = Bengine::ResourceManager::getTexture("Textures/light_bricks.png").id;
	GLuint glass = Bengine::ResourceManager::getTexture("Textures/glass.png").id;
	//GLuint redBricks = Bengine::ResourceManager::getTexture("Textures/red_bricks.png").id;

	// Render all the tiles
	for (int y = 0; y < m_levelData.size(); y++)
	{
		for (int x = 0; x < m_levelData[y].size(); x++)
		{
			char tile = m_levelData[y][x];

			// Get destination rect
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);

			// Process the tile
			switch (tile)
			{
			case 'R':
				m_spriteBatch.draw(destRect, uvRect, redBricks, 0.0f, whiteColor);
				break;
			case 'G':
				m_spriteBatch.draw(destRect, uvRect, glass, 0.0f, whiteColor);
				break;
			case 'L':
				m_spriteBatch.draw(destRect, uvRect, lightBricks, 0.0f, whiteColor);
				break;
			case '@':
				m_levelData[ y ][ x ] = '.';					/// So we don't collide with a Player @
				m_playerPosition.x = x * TILE_WIDTH;
				m_playerPosition.y = y * TILE_WIDTH;
				break;
			case 'Z':
				m_levelData[ y ][ x ] = '.';					/// So we don't collide with a Zombie Z
				m_zombiesPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
				break;
			case '.':
				break;
			default:
				std::printf("Unexpected symbol %c at (%d, %d)", tile, x, y);
				break;
			}
		}
	}

	m_spriteBatch.end();
}

Level::~Level()
{
}

void Level::printLevel()
{
	std::for_each(m_levelData.begin(), m_levelData.end(), [](std::string row) { std::cout << row << "\n"; });

	//for (auto row : _levelData)
	//{
	//	std::cout << row << "\n";
	//}
}

void Level::draw()
{
	m_spriteBatch.render();
}
