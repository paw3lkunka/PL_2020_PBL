#include "GamePlayModule.hpp"
#include "Message.inl"
#include "Core.hpp"
#include "ScenesPaths.hpp"

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
            pauseGame();
        break;
        
        case Event::LOAD_SCENE:
            reloadScene(msg.getValue<const char*>());
        break;
    }
}

void GamePlayModule::init()
{
    ObjectModule& om = GetCore().objectModule;

    info.format = GL_RGBA;
    info.generateMipmaps = false;
    info.magFilter = GL_LINEAR;
    info.minFilter = GL_LINEAR;
    info.wrapMode = GL_CLAMP_TO_EDGE;

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
    GetCore().callbacksModule.init("Health_Bar");
}

void GamePlayModule::initLoadingScreen(ObjectModule& om)
{
    loadingScreenEntity = new Entity(-1, 2, "LoadingScreen");
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
    pauseScreenEntity = new Entity(-2, 2, "PauseScreen");
    auto bg = om.newTexture("Resources/Sprites/paper.png", info);
    auto uiRenderer = new UiRenderer();
    uiRenderer->material = om.newMaterial(uiStandard, "PauseScreenMat", RenderType::Transparent);
    uiRenderer->material->setVec4("color", {1.0f, 1.0f, 1.0f, 0.8f});
    uiRenderer->material->setTexture("sprite", bg);

    pauseScreenEntity->addComponent(uiRenderer);

    auto rt = new RectTransform();
    rt->getAnchorModifiable() = {0.5f, 0.5f};
    rt->getSizeModifiable() = {350, 400};
    GetCore().uiModule.rootNodes.push_back(rt);
    pauseScreenEntity->addComponent(rt);

    {
        auto josefin = GetCore().objectModule.newFont("Resources/Fonts/JosefinSans-Regular.ttf", 128, "JosefinSans-Regular");
        auto courier = GetCore().objectModule.newFont("Resources/Fonts/CourierNew.ttf", 128, "CourierNew");
        auto textShader = GetCore().objectModule.newShader("TextStandard", "Resources/Shaders/TextStandard/TextStandard.vert", "Resources/Shaders/TextStandard/TextStandard.frag");
        {
            auto pauseText = new Entity(-3, 2, "pauseText");
            auto crt = new RectTransform();
                crt->getSizeModifiable() = {0.5f, 0.5f};
                crt->getLocalPositionModifiable() = {-92.0f, 140.0f};
                crt->setParent(pauseScreenEntity->getComponentPtr<RectTransform>());
                pauseText->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = GetCore().objectModule.newMaterial(textShader, "pauseText", RenderType::Transparent);
                cui->material->setVec4("color", {0.0f, 0.0f, 0.0f, 1.0f});
                cui->mesh.font = courier;
                cui->mesh.text = "Pause";
                pauseText->addComponent(cui);
        }

        auto rect_border = om.newTexture("Resources/Sprites/rect_border.png", info);
        glm::vec4 baseColor = {0.733f, 0.066f, 0.066f, 1.0f};
        glm::vec4 inactiveColor = {0.733f, 0.066f, 0.066f, 1.0f};
        glm::vec4 highlightedColor = {0.858f, 0.078f, 0.078f, 1.0f};
        glm::vec4 onClickColor = {0.654f, 0.066f, 0.066f, 1.0f};

        auto textMaterial = om.newMaterial(textShader, "buttonsText", RenderType::Transparent);
        textMaterial->setVec4("color", baseColor);
        {
            auto entity = new Entity(-5, 3, "Unpause_button");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {2, 24};
                crt->getSizeModifiable() = {250.0f, 100.0f};
                crt->getLocalRotationModifiable() = glm::radians(0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto ui = new UiRenderer();
                auto buttMaterial = om.newMaterial(uiStandard, "unpauseButtonMat", RenderType::Transparent);
                buttMaterial->setTexture("sprite", rect_border);
                buttMaterial->setVec4("color", {1.0f, 0.0f, 1.0f, 0.5f});
                ui->material = buttMaterial;
                entity->addComponent(ui);
            
            auto butt = new Button();
                butt->baseColor = baseColor;
                butt->inactiveColor = inactiveColor;
                butt->highlightedColor = highlightedColor;
                butt->onClickColor = onClickColor;
                butt->onClickEvents.push_back(Message(Event::PAUSE_GAME));
                butt->isActive = true;
                entity->addComponent(butt);
        }

        {
            auto entity = new Entity(-4, 2, "Unpause_button_text");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {-104.0f, 12.0f};
                crt->getSizeModifiable() = {0.35f, 0.35f};
                crt->getLocalRotationModifiable() = glm::radians(0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = textMaterial;
                cui->mesh.font = courier;
                cui->mesh.text = "Continue";
                entity->addComponent(cui);
        }

        {
            auto entity = new Entity(-6, 3, "Do_Exit_button");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {2, -110};
                crt->getSizeModifiable() = {250.0f, 100.0f};
                crt->getLocalRotationModifiable() = glm::radians(-0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto ui = new UiRenderer();
                auto buttMaterial = om.newMaterial(uiStandard, "doExitButtonMat", RenderType::Transparent);
                buttMaterial->setTexture("sprite", rect_border);
                buttMaterial->setVec4("color", {1.0f, 0.0f, 1.0f, 0.5f});
                ui->material = buttMaterial;
                entity->addComponent(ui);
            
            auto butt = new Button();
                butt->baseColor = baseColor;
                butt->inactiveColor = inactiveColor;
                butt->highlightedColor = highlightedColor;
                butt->onClickColor = onClickColor;
                butt->onClickEvents.push_back(Message(Event::LOAD_SCENE, Scenes::mainMenuScene));
                entity->addComponent(butt);
        }

        {
            auto entity = new Entity(-7, 2, "Do_Exit_button_text");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {-113.0f, -119.0f};
                crt->getSizeModifiable() = {0.25f, 0.25f};
                crt->getLocalRotationModifiable() = glm::radians(-0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = textMaterial;
                cui->mesh.font = courier;
                cui->mesh.text = "Exit to Menu";
                entity->addComponent(cui);
        }
    }   
}

void GamePlayModule::pauseGame()
{
    Core::instance->gamePaused = !Core::instance->gamePaused;
    Core::instance->messageBus.sendMessage(Message(Event::AUDIO_SOURCE_PAUSE_ALL_PLAYING));
    if(pauseScreenEntity != nullptr)
    {

        if(Core::instance->gamePaused)
        {
            GetCore().objectModule.objectContainer.entities.push_back(*pauseScreenEntity);
            if(pauseScreenEntity->getComponentPtr<RectTransform>() != nullptr)
            {
                for(auto pChildren : pauseScreenEntity->getComponentPtr<RectTransform>()->children)
                {
                    GetCore().objectModule.objectContainer.entities.push_back(*pChildren->entityPtr);
                }
            }
        }
        else
        {
            auto iter2 = std::find(GetCore().objectModule.objectContainer.entities.begin(), GetCore().objectModule.objectContainer.entities.end(), *pauseScreenEntity);
            if(iter2 != GetCore().objectModule.objectContainer.entities.end())
            {
                GetCore().objectModule.objectContainer.entities.erase(iter2);
            }
            if(pauseScreenEntity->getComponentPtr<RectTransform>() != nullptr)
            {
                for(auto pChildren : pauseScreenEntity->getComponentPtr<RectTransform>()->children)
                {
                    auto iters = std::find(GetCore().objectModule.objectContainer.entities.begin(), GetCore().objectModule.objectContainer.entities.end(), *pChildren->entityPtr);
                    if(iters != GetCore().objectModule.objectContainer.entities.end())
                    {
                        GetCore().objectModule.objectContainer.entities.erase(iters);
                    }
                }
            }
        }
    }
}   