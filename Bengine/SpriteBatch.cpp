#include <algorithm>

#include "SpriteBatch.h"

namespace Bengine
{
	SpriteBatch::SpriteBatch() : _vbo(0), _vao(0), _sortType(GlyphSortType::TEXTURE), _glyphs()
	{
	}

	SpriteBatch::~SpriteBatch()
	{
	}

	void SpriteBatch::init()
	{
		createVertexArray();
	}

	void SpriteBatch::begin(GlyphSortType sortType /* Default value = GlyphSortType::TEXTURE */)
	{
		_sortType = sortType;
		_renderBatches.clear();

		for (int i = 0; i < _glyphs.size(); i++)
		{
			delete _glyphs[i];
		}

		_glyphs.clear();
	}

	void SpriteBatch::end()
	{
		sortGlyphs();
		createRenderBatches();
	}

	void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, Bengine::ColorRGBA8& color)
	{
		Glyph* glyph = new Glyph;

		glyph->texture = texture;
		glyph->depth = depth;

		glyph->topLeft.color = color;
		glyph->topLeft.setPosition(destRect.x, destRect.y + destRect.w);
		glyph->topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

		glyph->bottomLeft.color = color;
		glyph->bottomLeft.setPosition(destRect.x, destRect.y);
		glyph->bottomLeft.setUV(uvRect.x, uvRect.y);

		glyph->topRight.color = color;
		glyph->topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
		glyph->topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		glyph->bottomRight.color = color;
		glyph->bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
		glyph->bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

		_glyphs.push_back(glyph);
	}

	void SpriteBatch::render()
	{
		glBindVertexArray(_vao);

		for (int i = 0; i < _renderBatches.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, _renderBatches[i]._texture);
			glDrawArrays(GL_TRIANGLES, _renderBatches[i]._offset, _renderBatches[i]._numVertices);
		}

		glBindVertexArray(0);
	}
	
	void SpriteBatch::createRenderBatches()
	{
		std::vector<Vertex> vertices;
		vertices.resize(_glyphs.size() * 6);

		if (_glyphs.empty())
		{
			return;
		}

		int offset = 0;
		int cv = 0;		// Current Vertex
		_renderBatches.emplace_back(offset, 6, _glyphs[0]->texture);
		vertices[cv++] = _glyphs[0]->topLeft;
		vertices[cv++] = _glyphs[0]->bottomLeft;
		vertices[cv++] = _glyphs[0]->bottomRight;

		vertices[cv++] = _glyphs[0]->bottomRight;
		vertices[cv++] = _glyphs[0]->topRight;
		vertices[cv++] = _glyphs[0]->topLeft;

		offset += 6;

		for (int cg = 1; cg < _glyphs.size(); cg++)	// Current Glyph
		{
			if (_glyphs[cg]->texture != _glyphs[cg-1]->texture)
			{
				_renderBatches.emplace_back(offset, 6, _glyphs[cg]->texture);
			}
			else
			{
				_renderBatches.back()._numVertices += 6;
			}

			vertices[cv++] = _glyphs[cg]->topLeft;
			vertices[cv++] = _glyphs[cg]->bottomLeft;
			vertices[cv++] = _glyphs[cg]->bottomRight;

			vertices[cv++] = _glyphs[cg]->bottomRight;
			vertices[cv++] = _glyphs[cg]->topRight;
			vertices[cv++] = _glyphs[cg]->topLeft;

			offset += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		// Orphan the buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		// Upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SpriteBatch::createVertexArray()
	{
		if (_vao ==0)
		{
			glGenVertexArrays(1, &_vao);
		}

		glBindVertexArray(_vao);

		if (_vbo == 0)
		{
			glGenBuffers(1, &_vbo);
		}

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		// Tell OpenGL we want to use the first attribute array
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// This is the position attribute pointer
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// This is the color attribute pointer
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		// This is the UV attribute pointer
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		glBindVertexArray(0);
	}

	void SpriteBatch::sortGlyphs()
	{
		switch (_sortType)
		{
		case Bengine::GlyphSortType::NONE:
			break;
		case Bengine::GlyphSortType::FRONT_TO_BACK:
			std::stable_sort(_glyphs.begin(), _glyphs.end(), compareFrontToBack);
			break;
		case Bengine::GlyphSortType::BACK_TO_FRONT:
			std::stable_sort(_glyphs.begin(), _glyphs.end(), compareBackToFront);
			break;
		case Bengine::GlyphSortType::TEXTURE:
			std::stable_sort(_glyphs.begin(), _glyphs.end(), compareTexture);
			break;
		default:
			break;
		}
	}
	bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph* b)
	{
		return (a->depth < b->depth);
	}

	bool SpriteBatch::compareBackToFront(Glyph* a, Glyph* b)
	{
		return (a->depth > b->depth);
	}

	bool SpriteBatch::compareTexture(Glyph* a, Glyph* b)
	{
		return (a->texture < b->texture);
	}
}