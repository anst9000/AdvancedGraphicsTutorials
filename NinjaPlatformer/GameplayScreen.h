#pragma once

#include <vector>
#include <memory>

#include <Bengine/IGameScreen.h>
#include <box2d/box2d.h>
#include <Bengine/SpriteBatch.h>
#include <Bengine/GLSLProgram.h>
#include <Bengine/Camera2D.h>
#include <Bengine/GLTexture.h>
#include <Bengine/Window.h>
#include <Bengine/DebugRenderer.h>
#include <Bengine/GUI.h>

#include "Box.h"
#include "Player.h"


// Our custom gameplay screen that inherits from the IGameScreen
class GameplayScreen : public Bengine::IGameScreen
{
public:
    GameplayScreen( Bengine::Window* window );
    ~GameplayScreen();

    virtual int getNextScreenIndex() const override;

    virtual int getPreviousScreenIndex() const override;

    virtual void build() override;

    virtual void destroy() override;

    virtual void onEntry() override;

    virtual void onExit() override;

    virtual void update() override;

    virtual void draw() override;

private:
    void initUI();
    void checkInput();
    void renderDebug();
    void renderLight();

    bool onExitClicked( const CEGUI::EventArgs& e );

    Bengine::SpriteBatch m_spriteBatch;
    Bengine::GLSLProgram m_textureProgram;
    Bengine::GLSLProgram m_lightProgram;
    Bengine::Camera2D m_camera;
    Bengine::GLTexture m_texture;
    Bengine::Window* m_window;
    Bengine::DebugRenderer m_debugRenderer;
    Bengine::GUI m_gui;

    bool m_renderDebug = false;
    bool m_renderLight = false;

    Player m_player;
    std::vector<Box> m_boxes;
    std::unique_ptr<b2World> m_world;
};

