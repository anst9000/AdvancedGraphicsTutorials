#include "SpriteBatch.h"

#include <algorithm>

namespace Bengine {


    Glyph::Glyph( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint Texture, float Depth, const ColorRGBA8& color )
        : texture( Texture ), depth( Depth )
    {
        topLeft.color = color;
        topLeft.setPosition( destRect.x, destRect.y + destRect.w );
        topLeft.setUV( uvRect.x, uvRect.y + uvRect.w );

        bottomLeft.color = color;
        bottomLeft.setPosition( destRect.x, destRect.y );
        bottomLeft.setUV( uvRect.x, uvRect.y );

        bottomRight.color = color;
        bottomRight.setPosition( destRect.x + destRect.z, destRect.y );
        bottomRight.setUV( uvRect.x + uvRect.z, uvRect.y );

        topRight.color = color;
        topRight.setPosition( destRect.x + destRect.z, destRect.y + destRect.w );
        topRight.setUV( uvRect.x + uvRect.z, uvRect.y + uvRect.w );
    }

    Glyph::Glyph( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint Texture, float Depth, const ColorRGBA8& color, float angle )
        : texture( Texture ), depth( Depth )
    {
        glm::vec2 halfDims( destRect.z / 2.0f, destRect.w / 2.0f );

        // Get points centered at origin
        glm::vec2 tl( -halfDims.x, halfDims.y );
        glm::vec2 bl( -halfDims.x, -halfDims.y );
        glm::vec2 br( halfDims.x, -halfDims.y );
        glm::vec2 tr( halfDims.x, halfDims.y );

        // Rotate the points
        tl = rotatePoint( tl, angle ) + halfDims;
        bl = rotatePoint( bl, angle ) + halfDims;
        br = rotatePoint( br, angle ) + halfDims;
        tr = rotatePoint( tr, angle ) + halfDims;

        topLeft.color = color;
        topLeft.setPosition( destRect.x + tl.x, destRect.y + tl.y );
        topLeft.setUV( uvRect.x, uvRect.y + uvRect.w );

        bottomLeft.color = color;
        bottomLeft.setPosition( destRect.x + bl.x, destRect.y + bl.y );
        bottomLeft.setUV( uvRect.x, uvRect.y );

        bottomRight.color = color;
        bottomRight.setPosition( destRect.x + br.x, destRect.y + br.y );
        bottomRight.setUV( uvRect.x + uvRect.z, uvRect.y );

        topRight.color = color;
        topRight.setPosition( destRect.x + tr.x, destRect.y + tr.y );
        topRight.setUV( uvRect.x + uvRect.z, uvRect.y + uvRect.w );
    }

    glm::vec2 Glyph::rotatePoint( const glm::vec2& pos, float angle )
    {
        glm::vec2 newv;
        newv.x = pos.x * cos( angle ) - pos.y * sin( angle );
        newv.y = pos.x * sin( angle ) + pos.y * cos( angle );

        return newv;
    }

    SpriteBatch::SpriteBatch()
    {
        // Empty
    }

    SpriteBatch::~SpriteBatch()
    {
        // Empty
    }

    void SpriteBatch::init()
    {
        createVertexArray();
    }

    void SpriteBatch::dispose()
    {
        if ( m_vao != 0 )
        {
            glDeleteVertexArrays( 1, &m_vao );
            m_vao = 0;
        }

        if ( m_vbo != 0 )
        {
            glDeleteBuffers( 1, &m_vbo );
            m_vbo = 0;
        }
    }

    void SpriteBatch::begin( GlyphSortType sortType /* GlyphSortType::TEXTURE */ )
    {
        m_sortType = sortType;
        m_renderBatches.clear();

        // Makes _glpyhs.size() == 0, however it does not free internal memory.
        // So when we later call emplace_back it doesn't need to internally call new.
        m_glyphs.clear();
    }

    void SpriteBatch::end()
    {
        // Set up all pointers for fast sorting
        m_glyphPointers.resize( m_glyphs.size() );

        for ( size_t i = 0; i < m_glyphs.size(); i++ )
        {
            m_glyphPointers[ i ] = &m_glyphs[ i ];
        }

        sortGlyphs();
        createRenderBatches();
    }

    void SpriteBatch::draw( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color )
    {
        m_glyphs.emplace_back( destRect, uvRect, texture, depth, color );
    }

    void SpriteBatch::draw( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, float angle ) 
    {
        m_glyphs.emplace_back( destRect, uvRect, texture, depth, color, angle );
    }

    void SpriteBatch::draw( const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const ColorRGBA8& color, const glm::vec2& direction )
    {
        const glm::vec2 right( 1.0f, 0.0f );
        float angle = acos( glm::dot( right, direction ) );
    
        if ( direction.y < 0.0f ) angle = -angle;

        m_glyphs.emplace_back( destRect, uvRect, texture, depth, color, angle );
    }

    void SpriteBatch::render()
    {
        // Bind our VAO. This sets up the opengl state we need, including the 
        // vertex attribute pointers and it binds the VBO
        glBindVertexArray( m_vao );

        for ( size_t i = 0; i < m_renderBatches.size(); i++ )
        {
            glBindTexture( GL_TEXTURE_2D, m_renderBatches[ i ].texture );

            glDrawArrays( GL_TRIANGLES, m_renderBatches[ i ].offset, m_renderBatches[ i ].numVertices );
        }

        glBindVertexArray( 0 );
    }

    void SpriteBatch::createRenderBatches()
    {
        // This will store all the vertices that we need to upload
        std::vector <Vertex> vertices;
        // Resize the buffer to the exact size we need so we can treat
        // it like an array
        vertices.resize( m_glyphPointers.size() * 6 );

        if ( m_glyphPointers.empty() )
        {
            return;
        }

        int offset = 0; // current offset
        int cv = 0; // current vertex

        //Add the first batch
        m_renderBatches.emplace_back( offset, 6, m_glyphPointers[ 0 ]->texture );
        vertices[ cv++ ] = m_glyphPointers[ 0 ]->topLeft;
        vertices[ cv++ ] = m_glyphPointers[ 0 ]->bottomLeft;
        vertices[ cv++ ] = m_glyphPointers[ 0 ]->bottomRight;
        vertices[ cv++ ] = m_glyphPointers[ 0 ]->bottomRight;
        vertices[ cv++ ] = m_glyphPointers[ 0 ]->topRight;
        vertices[ cv++ ] = m_glyphPointers[ 0 ]->topLeft;
        offset += 6;

        //Add all the rest of the glyphs
        for ( size_t cg = 1; cg < m_glyphPointers.size(); cg++ )
        {

            // Check if this glyph can be part of the current batch
            if ( m_glyphPointers[ cg ]->texture != m_glyphPointers[ cg - 1 ]->texture )
            {
                // Make a new batch
                m_renderBatches.emplace_back( offset, 6, m_glyphPointers[ cg ]->texture );
            }
            else
            {
                // If its part of the current batch, just increase numVertices
                m_renderBatches.back().numVertices += 6;
            }

            vertices[ cv++ ] = m_glyphPointers[ cg ]->topLeft;
            vertices[ cv++ ] = m_glyphPointers[ cg ]->bottomLeft;
            vertices[ cv++ ] = m_glyphPointers[ cg ]->bottomRight;
            vertices[ cv++ ] = m_glyphPointers[ cg ]->bottomRight;
            vertices[ cv++ ] = m_glyphPointers[ cg ]->topRight;
            vertices[ cv++ ] = m_glyphPointers[ cg ]->topLeft;
            offset += 6;
        }

        // Bind our VBO
        glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
        // Orphan the buffer (for speed)
        glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( Vertex ), nullptr, GL_DYNAMIC_DRAW );
        // Upload the data
        glBufferSubData( GL_ARRAY_BUFFER, 0, vertices.size() * sizeof( Vertex ), vertices.data() );

        // Unbind the VBO
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

    }

    void SpriteBatch::createVertexArray()
    {

        // Generate the VAO if it isn't already generated
        if ( m_vao == 0 )
        {
            glGenVertexArrays( 1, &m_vao );
        }

        // Bind the VAO. All subsequent opengl calls will modify it's state.
        glBindVertexArray( m_vao );

        //G enerate the VBO if it isn't already generated
        if ( m_vbo == 0 )
        {
            glGenBuffers( 1, &m_vbo );
        }

        glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

        //Tell opengl what attribute arrays we need
        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glEnableVertexAttribArray( 2 );

        //This is the position attribute pointer
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)offsetof( Vertex, position ) );
        //This is the color attribute pointer
        glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( Vertex ), (void*)offsetof( Vertex, color ) );
        //This is the UV attribute pointer
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)offsetof( Vertex, uv ) );

        glBindVertexArray( 0 );

    }

    void SpriteBatch::sortGlyphs()
    {
        switch ( m_sortType )
        {
        case GlyphSortType::BACK_TO_FRONT:
            std::stable_sort( m_glyphPointers.begin(), m_glyphPointers.end(), compareBackToFront );
            break;
        case GlyphSortType::FRONT_TO_BACK:
            std::stable_sort( m_glyphPointers.begin(), m_glyphPointers.end(), compareFrontToBack );
            break;
        case GlyphSortType::TEXTURE:
            std::stable_sort( m_glyphPointers.begin(), m_glyphPointers.end(), compareTexture );
            break;
        case GlyphSortType::NONE:
            break;
        }
    }

    bool SpriteBatch::compareFrontToBack( Glyph* a, Glyph* b )
    {
        return ( a->depth < b->depth );
    }

    bool SpriteBatch::compareBackToFront( Glyph* a, Glyph* b )
    {
        return ( a->depth > b->depth );
    }

    bool SpriteBatch::compareTexture( Glyph* a, Glyph* b )
    {
        return ( a->texture < b->texture );
    }

}