#include <SDL/SDL.h>

#include <iostream>
#include <random>

#include <Bengine/IMainGame.h>
#include <Bengine/ResourceManager.h>

#include "GameplayScreen.h"

GameplayScreen::GameplayScreen( Bengine::Window* window )
    : m_window( window )
{
    //m_screenIndex = SCREEN_INDEX_GAMEPLAY;
}

GameplayScreen::~GameplayScreen()
{
    delete m_window;
}

int GameplayScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

int GameplayScreen::getPreviousScreenIndex() const
{
    //return SCREEN_INDEX_MAINMENU;
    return SCREEN_INDEX_NO_SCREEN;
}

void GameplayScreen::build()
{
}

void GameplayScreen::destroy()
{
}

void GameplayScreen::onEntry()
{
    b2Vec2 gravity( 0.0f, -35.0f );
    m_world = std::make_unique<b2World>( gravity );

    m_debugRenderer.init();

    // Make the ground
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set( 0.0f, -18.0f );
    b2Body* groundBody = m_world->CreateBody( &groundBodyDef );

    // Make the ground fixture
    b2PolygonShape groundBox;
    groundBox.SetAsBox( 50.0f, 10.0f );
    groundBody->CreateFixture( &groundBox, 0.0f );

    // Load the texture
    m_texture = Bengine::ResourceManager::getTexture( "Assets/bricks_top.png" );

    // Make a bunch of boxes
    std::mt19937 rg;
    std::uniform_real_distribution<float> xPos( -10.0f, 10.0f );
    std::uniform_real_distribution<float> yPos( -10.0f, 15.0f );
    std::uniform_real_distribution<float> size( 0.5f, 2.5f );
    std::uniform_int_distribution<int> color( 0, 255 );
    const int NUM_BOXES = 50;


    for ( size_t i = 0; i < NUM_BOXES; i++ )
    {
        Bengine::ColorRGBA8 boxColor( color( rg ), color( rg ), color( rg ), 255 );
        Box newBox;
        float boxWidth = size( rg );
        float boxHeight = size( rg );
        newBox.init( m_world.get(), glm::vec2( xPos( rg ), yPos( rg ) ), glm::vec2( boxWidth, boxHeight ), m_texture, boxColor, false );
        m_boxes.push_back( newBox );
    }

    // Initialize spritebatch
    m_spriteBatch.init();

    // Shader init
    // Compile our texture
    m_textureProgram.compileShaders( "Shaders/textureShading.vert", "Shaders/textureShading.frag" );
    m_textureProgram.addAttribute( "vertexPosition" );
    m_textureProgram.addAttribute( "vertexColor" );
    m_textureProgram.addAttribute( "vertexUV" );
    m_textureProgram.linkShaders();

    // Init camera
    m_camera.init( m_window->getScreenWidth(), m_window->getScreenHeight() );
    m_camera.setScale( 32.0f );

    // Init player
    m_player.init( m_world.get(), glm::vec2( 0.0f, 15.0f ), glm::vec2( 1.0f, 1.5f ), Bengine::ColorRGBA8( 255, 255, 255, 255 ) );
}

void GameplayScreen::onExit()
{
    m_debugRenderer.dispose();
}

void GameplayScreen::update()
{
    m_camera.update();
    checkInput();
    m_player.update( m_game->inputManager );

    // Update the physics simulation
    m_world->Step( 1.0f / 60.0f, 6, 2 );
}


void GameplayScreen::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    m_textureProgram.use();

    // Upload texture uniform
    GLint textureUniform = m_textureProgram.getUniformLocation( "mySampler" );
    glUniform1i( textureUniform, 0 );
    glActiveTexture( GL_TEXTURE0 );

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation( "P" );
    glUniformMatrix4fv( pUniform, 1, GL_FALSE, &projectionMatrix[ 0 ][ 0 ] );

    m_spriteBatch.begin();

    // Draw all the boxes
    for ( auto& b : m_boxes )
    {
        b.draw( m_spriteBatch );
    }

    m_player.draw( m_spriteBatch );

    m_spriteBatch.end();
    m_spriteBatch.render();

    m_textureProgram.unuse();

    // Debug rendering
    if ( m_renderDebug )
    {
        glm::vec4 destRect;

        for ( auto& b : m_boxes )
        {
            //destRect.x = b.getBody()->GetPosition().x - b.getDimensions().x / 2.0f;
            //destRect.y = b.getBody()->GetPosition().y - b.getDimensions().y / 2.0f;
            //destRect.z = b.getDimensions().x;
            //destRect.w = b.getDimensions().y;
            //m_debugRenderer.drawBox( destRect, Bengine::ColorRGBA8( 255, 255, 255, 255 ), b.getBody()->GetAngle() );
            m_debugRenderer.drawCircle( glm::vec2( b.getBody()->GetPosition().x, b.getBody()->GetPosition().y ),
                Bengine::ColorRGBA8( 255, 255, 255, 255 ), b.getDimensions().x / 2.0f );
        }

        // Render player
        auto b = m_player.getBox();
        destRect.x = b.getBody()->GetPosition().x - b.getDimensions().x / 2.0f;
        destRect.y = b.getBody()->GetPosition().y - b.getDimensions().y / 2.0f;
        destRect.z = b.getDimensions().x;
        destRect.w = b.getDimensions().y;
        m_debugRenderer.drawBox( destRect, Bengine::ColorRGBA8( 255, 255, 255, 255 ), b.getBody()->GetAngle() );

        m_debugRenderer.end();
        m_debugRenderer.render( projectionMatrix, 2.0f );
    }
}

void GameplayScreen::checkInput() {
    SDL_Event evnt;

    while ( SDL_PollEvent( &evnt ) )
    {
        m_game->onSDLEvent( evnt );
    }
}
