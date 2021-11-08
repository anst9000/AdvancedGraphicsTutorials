#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "Vertex.h"

namespace Bengine {

    // Determines how we should sort the glyphs
    enum class GlyphSortType {
        NONE,
        FRONT_TO_BACK,
        BACK_TO_FRONT,
        TEXTURE
    };

    // A glyph is a single quad. These are added via SpriteBatch::draw
    class Glyph {
    public:
        Glyph() {};
        Glyph( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint Texture, float Depth, const ColorRGBA8& color );
        Glyph( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint Texture, float Depth, const ColorRGBA8& color, float angle );

        GLuint texture { 0 };
        float depth { 0.0f };

        Vertex topLeft;
        Vertex bottomLeft;
        Vertex topRight;
        Vertex bottomRight;

    private:
        // Rotates a point about (0,0) by angle
        glm::vec2 rotatePoint( const glm::vec2& pos, float angle );
    };

    // Each render batch is used for a single draw call
    class RenderBatch
    {
    public:
        RenderBatch( GLuint Offset, GLuint NumVertices, GLuint Texture ) : offset( Offset ),
            numVertices( NumVertices ), texture( Texture )
        { }

        GLuint offset;
        GLuint numVertices;
        GLuint texture;
    };

    // The SpriteBatch class is a more efficient way of drawing sprites
    class SpriteBatch
    {
    public:
        SpriteBatch();
        ~SpriteBatch();

        // Initializes the spritebatch
        void init();
        void dispose();

        // Begins the spritebatch
        void begin( GlyphSortType sortType = GlyphSortType::TEXTURE );

        // Ends the spritebatch
        void end();

        // Adds a glyph to the spritebatch
        void draw( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color );
        // Adds a glyph to the spritebatch with rotation
        void draw( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, float angle );
        // Adds a glyph to the spritebatch with rotation
        void draw( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, const glm::vec2& direction );

        // Renders the entire SpriteBatch to the screen
        void render();

    private:
        // Creates all the needed RenderBatches
        void createRenderBatches();

        // Generates our VAO and VBO
        void createVertexArray();

        // Sorts glyphs according to m_sortType
        void sortGlyphs();

        // Comparators used by sortGlyphs()
        static bool compareFrontToBack( Glyph* a, Glyph* b );
        static bool compareBackToFront( Glyph* a, Glyph* b );
        static bool compareTexture( Glyph* a, Glyph* b );

        GLuint m_vbo { 0 };
        GLuint m_vao { 0 };

        GlyphSortType m_sortType { GlyphSortType::NONE };

        std::vector<Glyph*> m_glyphPointers {};            ///< This is for sorting
        std::vector<Glyph> m_glyphs {};                    ///< These are the actual glyphs
        std::vector<RenderBatch> m_renderBatches {};
    };

}