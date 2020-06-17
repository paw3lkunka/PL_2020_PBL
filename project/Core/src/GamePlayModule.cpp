#include "GamePlayModule.hpp"
#include "Message.inl"
#include "Core.hpp"

#include "ECS.inc"

void GamePlayModule::receiveMessage(Message msg)
{
    switch(msg.getEvent())
    {
        case Event::EXIT_GAME:
            Core::instance->close();
        break;

        case Event::KEY_PRESSED:
            if(msg.getValue<int>() == GLFW_KEY_ESCAPE)
            {
                Core::instance->messageBus.sendMessage(Message(Event::EXIT_GAME));
            }
            if(msg.getValue<int>() == GLFW_KEY_P)
            {
                Core::instance->messageBus.sendMessage(Message(Event::PAUSE_GAME));
            }
        break;

        case Event::PAUSE_GAME:
            Core::instance->gamePaused = !Core::instance->gamePaused;
            Core::instance->messageBus.sendMessage(Message(Event::AUDIO_SOURCE_PAUSE_ALL_PLAYING));
        break;
        
        case Event::LOAD_SCENE:
            reloadScene(msg.getValue<const char*>());
        break;
    }
}

void GamePlayModule::init()
{
    ObjectModule& om = GetCore().objectModule;
    initLoadingScreen(om);
    initPauseScreen(om);
}

void GamePlayModule::reloadScene(std::string name)
{
    //TODO MAKE ONSCENELOAD & ONSCENE UNLOAD IN CORE

    GetCore().uiModule.rootNodes.push_back(loadingScreenEntity->getComponentPtr<RectTransform>());
    GetCore().objectModule.objectContainer.entities.push_back(*loadingScreenEntity);
    GetCore().uiModule.updateRectTransforms();
    GetCore().gameSystemsModule.run(System::FRAME);
    GetCore().messageBus.notify();
    GetCore().rendererModule.render();
    GetCore().inputModule.clearFlags();

    GetCore().objectModule.unloadSceneAndLoadNew(name);
    
    
    GetCore().messageBus.notify();
    
    GetCore().detectionBarSystem.init("DetectionProgressBar");
    GetCore().callbacksModule.init("Health_Bar", 30.0f);
}

void GamePlayModule::initLoadingScreen(ObjectModule& om)
{
    loadingScreenEntity = new Entity(-1, 2, "LoadingScreen");
    info.format = GL_RGBA;
    info.generateMipmaps = false;
    info.magFilter = GL_LINEAR;
    info.minFilter = GL_LINEAR;
    info.wrapMode = GL_CLAMP_TO_EDGE;
    auto tex = om.newTexture("Resources/Sprites/loding.png", info);
    auto uiRenderer = new UiRenderer();
    uiStandard = om.newShader("UiStandard", "Resources/Shaders/UiStandard/UiStandard.vert", "Resources/Shaders/UiStandard/UiStandard.frag");
    uiRenderer->material = om.newMaterial(uiStandard, "LoadingScreenMat", RenderType::Transparent);
    uiRenderer->material->setVec4("color", {1.0f, 1.0f, 1.0f, 0.8f});
    uiRenderer->material->setTexture("sprite", tex);

    loadingScreenEntity->addComponent(uiRenderer);
    auto rt = new RectTransform();
    rt->getAnchorModifiable() = {0.5f, 0.5f};
    rt->getSizeModifiable() = {1920, 1080};
    loadingScreenEntity->addComponent(rt);
}

void GamePlayModule::initPauseScreen(ObjectModule& om)
{

}
