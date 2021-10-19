#include "Level.h"
#include <Bengine/Errors.h>
#include <Bengine/ResourceManager.h>

#include <fstream>
#include <iostream>
#include <algorithm>

Level::Level(const std::string& fileName) :
	_levelData(),
	_numHumans(),
	_spriteBatch(),
	_playerPosition(),
	_zombiesPositions()
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
	readIn >> tmp >> _numHumans;

	std::string line;
	std::getline( readIn, line );

	while (std::getline(readIn, line))
	{
		_levelData.push_back(line);
	}

	//// Print the level
	//printLevel();

	_spriteBatch.init();
	_spriteBatch.begin();

	glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	Bengine::ColorRGBA8 whiteColor( 255, 255, 255, 255 );
	
	GLuint redBricks = Bengine::ResourceManager::getTexture("Textures/red_bricks.png").id;
	GLuint lightBricks = Bengine::ResourceManager::getTexture("Textures/light_bricks.png").id;
	GLuint glass = Bengine::ResourceManager::getTexture("Textures/glass.png").id;
	//GLuint redBricks = Bengine::ResourceManager::getTexture("Textures/red_bricks.png").id;

	// Render all the tiles
	for (int y = 0; y < _levelData.size(); y++)
	{
		for (int x = 0; x < _levelData[y].size(); x++)
		{
			char tile = _levelData[y][x];

			// Get destination rect
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);

			// Process the tile
			switch (tile)
			{
			case 'R':
				_spriteBatch.draw(destRect, uvRect, redBricks, 0.0f, whiteColor);
				break;
			case 'G':
				_spriteBatch.draw(destRect, uvRect, glass, 0.0f, whiteColor);
				break;
			case 'L':
				_spriteBatch.draw(destRect, uvRect, lightBricks, 0.0f, whiteColor);
				break;
			case '@':
				_levelData[ y ][ x ] = '.';					/// So we don't collide with a Player @
				_playerPosition.x = x * TILE_WIDTH;
				_playerPosition.y = y * TILE_WIDTH;
				break;
			case 'Z':
				_levelData[ y ][ x ] = '.';					/// So we don't collide with a Zombie Z
				_zombiesPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
				break;
			case '.':
				break;
			default:
				std::printf("Unexpected symbol %c at (%d, %d)", tile, x, y);
				break;
			}
		}
	}

	_spriteBatch.end();
}

Level::~Level()
{
}

void Level::printLevel()
{
	std::for_each(_levelData.begin(), _levelData.end(), [](std::string row) { std::cout << row << "\n"; });

	//for (auto row : _levelData)
	//{
	//	std::cout << row << "\n";
	//}
}

void Level::draw()
{
	_spriteBatch.render();
}
