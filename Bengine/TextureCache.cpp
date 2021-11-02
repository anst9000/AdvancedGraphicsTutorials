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
		auto mit = m_textureMap.find(texturePath);

		// Check if its not int the map
		if (mit == m_textureMap.end())
		{
			GLTexture newTexture = ImageLoader::loadPNG(texturePath);

			//std::pair<std::string, GLTexture> newPair(texturePath, newTexture);
			//m_textureMap.insert(newPair);
			m_textureMap.insert(std::make_pair(texturePath, newTexture));

			//std::cout << "Loaded Texture.\n";

			return newTexture;
		}

		//std::cout << "Used Cached Texture.\n";

		return mit->second;
	}

}