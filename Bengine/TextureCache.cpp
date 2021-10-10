#include "TextureCache.h"
#include "ImageLoader.h"
#include <iostream>

namespace Bengine
{

	TextureCache::TextureCache()
	{
	}

	TextureCache::~TextureCache()
	{
	}

	GLTexture TextureCache::getTexture(std::string texturePath)
	{
		// Lookup the texture and see if its in the map
		auto mit = _textureMap.find(texturePath);

		// Check if its not int the map
		if (mit == _textureMap.end())
		{
			GLTexture newTexture = ImageLoader::loadPNG(texturePath);

			//std::pair<std::string, GLTexture> newPair(texturePath, newTexture);
			//_textureMap.insert(newPair);
			_textureMap.insert(std::make_pair(texturePath, newTexture));

			std::cout << "Loaded Texture.\n";

			return newTexture;
		}

		std::cout << "Used Cached Texture.\n";

		return mit->second;
	}

}