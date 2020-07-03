#include "GamePlayModule.hpp"
#include "Message.inl"
#include "Core.hpp"
#include "ScenesPaths.hpp"
#include "EnemyDataStructures.inl"

#include "ECS.inc"

void GamePlayModule::receiveMessage(Message msg)
{
    switch(msg.getEvent())
    {
        case Event::COLLISION_ENTER:
            if (Kayak::get() != nullptr)
            {
                auto data = msg.getValue<CollisionData>();
                bool valid = false;
                valid |= data.body1->entityPtr == Kayak::get()->entityPtr;
                valid |= data.body2->entityPtr == Kayak::get()->entityPtr;
                if (valid)
                {
                    AudioSource* sound;
                    if(data.body1->entityPtr == Kayak::get()->entityPtr)
                    {
                        sound = data.body2->entityPtr->getComponentPtr<AudioSource>();
                    }
                    else
                    {
                        sound = data.body2->entityPtr->getComponentPtr<AudioSource>();
                    }

                    sound->getGainModifiable() = glm::length( Kayak::get()->entityPtr->getComponentPtr<Rigidbody>()->velocity ) / 25.0f;
                    GetCore().messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY_ONE_SHOT, sound) );

                    rocksHit();
                }
            }
        break;
        
        case Event::TRIGGER_ENTER:
        {
            auto data = msg.getValue<TriggerData>();

            if (data.causeBody->entityPtr->getComponentPtr<Kayak>()
             && data.triggerBody->entityPtr->getComponentPtr<Finish>())
            {
                togglePauseState();
                summarize();
            }

            if (data.causeBody->entityPtr->getComponentPtr<Kayak>()
             && data.triggerBody->entityPtr->getComponentPtr<Hideout>())
            {
                auto hideoutSo = data.triggerBody->entityPtr->getComponentPtr<AudioSource>();
                GetCore().messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY, hideoutSo) );
            }
        }
        break;

        case Event::TRIGGER_EXIT:
        {
            auto data = msg.getValue<TriggerData>();

            if (data.causeBody->entityPtr->getComponentPtr<Kayak>()
             && data.triggerBody->entityPtr->getComponentPtr<Hideout>())
            {
                auto hideoutSo = data.triggerBody->entityPtr->getComponentPtr<AudioSource>();
                GetCore().messageBus.sendMessage( Message(Event::AUDIO_SOURCE_STOP, hideoutSo) );
            }
        }
        break;

        case Event::PLAYER_ATTACKED:
            if (msg.getValue<AttackData>().success)
            {
                bulletHit();
            }
        break;

        case Event::EXIT_GAME:
            Core::instance->close();
        break;

        case Event::KEY_PRESSED:
            if(msg.getValue<int>() == GLFW_KEY_ESCAPE)
            {
                Core::instance->messageBus.sendMessage(Message(Event::PAUSE_GAME));
            }
        break;

        case Event::PAUSE_GAME:
            togglePauseState();
            pauseScreen();
        break;

        case Event::HP_0:
        {
            auto sound = GetCore().objectModule.getEntityPtrByName("HP_0_SOUND")->getComponentPtr<AudioSource>();
            if(sound)
            {
                GetCore().messageBus.sendMessage( Message(Event::AUDIO_SOURCE_PLAY_ONE_SHOT, sound) );
            }

            auto detectionSound = GetCore().objectModule.getEntityPtrByName("DETECTION_SOUND")->getComponentPtr<AudioSource>();
            if(detectionSound)
            {
                GetCore().messageBus.sendMessage( Message(Event::AUDIO_SOURCE_STOP, detectionSound) );
            }
            
            togglePauseState();
            youDied();
        }
        break;
        
        case Event::LOAD_SCENE:
            if(Core::instance->gamePaused)
            {
                togglePauseState();
            }
            reloadScene(msg.getValue<const char*>());
        break;
    }
}

void GamePlayModule::init(const char* hpBarName, float bulletDamage)
{
    Entity* e = GetCore().objectModule.getEntityPtrByName(hpBarName);
    if (e)
    {
        healthbarPtr = e->getComponentPtr<ProgressBar>();
    }

    this->bulletDamage = bulletDamage;
    
    for (size_t i = 0; i < 18; i++)
    {
        goodCargoEntities[i]->getComponentPtr<TextRenderer>()->mesh.text = "";
        badCargoEntities[i]->getComponentPtr<TextRenderer>()->mesh.text = "";
    }
}

void GamePlayModule::initScreens()
{
    ObjectModule& om = GetCore().objectModule;

    info.format = GL_RGBA;
    info.generateMipmaps = false;
    info.magFilter = GL_LINEAR;
    info.minFilter = GL_LINEAR;
    info.wrapMode = GL_CLAMP_TO_EDGE;

    initLoadingScreen(om);
    initPauseScreen(om);
    initYouDieScreen(om);
    initSummaryScreen(om);
}

void GamePlayModule::reloadScene(std::string name)
{
    GetCore().uiModule.rootNodes.push_back(loadingScreenEntity->getComponentPtr<RectTransform>());
    GetCore().objectModule.objectContainer.entities.push_back(*loadingScreenEntity);
    GetCore().uiModule.updateRectTransforms();
    GetCore().gameSystemsModule.run(System::FRAME);
    GetCore().messageBus.notify();
    GetCore().rendererModule.render();
    healthbarPtr = nullptr;

    GetCore().sceneUnload();
    // ! clear message bus, for omitting messages between scenes
    GetCore().messageBus.clearBuffers();
    GetCore().objectModule.unloadSceneAndLoadNew(name);
    GetCore().sceneInit();   
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
    pauseScreenEntity->addComponent(rt);

    {
        josefin = GetCore().objectModule.newFont("Resources/Fonts/JosefinSans-Regular.ttf", 128, "JosefinSans-Regular");
        courier = GetCore().objectModule.newFont("Resources/Fonts/CourierNew.ttf", 128, "CourierNew");
        textShader = GetCore().objectModule.newShader("TextStandard", "Resources/Shaders/TextStandard/TextStandard.vert", "Resources/Shaders/TextStandard/TextStandard.frag");
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

void GamePlayModule::initYouDieScreen(ObjectModule& om)
{
    youDiedScreenEntity = new Entity(-8, 2, "DieScreen");
    auto bg = om.newTexture("Resources/Sprites/paper.png", info);
    auto uiRenderer = new UiRenderer();
    uiRenderer->material = om.newMaterial(uiStandard, "DieScreenMat", RenderType::Transparent);
    uiRenderer->material->setVec4("color", {1.0f, 1.0f, 1.0f, 0.8f});
    uiRenderer->material->setTexture("sprite", bg);

    youDiedScreenEntity->addComponent(uiRenderer);

    auto rt = new RectTransform();
    rt->getAnchorModifiable() = {0.5f, 0.5f};
    rt->getSizeModifiable() = {350, 400};
    youDiedScreenEntity->addComponent(rt);

    {
        {
            auto pauseText = new Entity(-9, 2, "youDiedText");
            auto crt = new RectTransform();
                crt->getSizeModifiable() = {0.5f, 0.5f};
                crt->getLocalPositionModifiable() = {-152.0f, 128.0f};
                crt->setParent(youDiedScreenEntity->getComponentPtr<RectTransform>());
                pauseText->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = GetCore().objectModule.newMaterial(textShader, "pauseText", RenderType::Transparent);
                cui->material->setVec4("color", {0.0f, 0.0f, 0.0f, 1.0f});
                cui->mesh.font = courier;
                cui->mesh.text = "You Died";
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
            auto entity = new Entity(-10, 3, "ToMenuButton");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {2, 24};
                crt->getSizeModifiable() = {250.0f, 100.0f};
                crt->getLocalRotationModifiable() = glm::radians(0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto ui = new UiRenderer();
                auto buttMaterial = om.newMaterial(uiStandard, "ToMenuButtonMat", RenderType::Transparent);
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
                butt->isActive = true;
                entity->addComponent(butt);
        }

        {
            auto entity = new Entity(-11, 2, "ToMenuButtonText");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {-113, 16};
                crt->getSizeModifiable() = {0.25f, 0.25f};
                crt->getLocalRotationModifiable() = glm::radians(0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = textMaterial;
                cui->mesh.font = courier;
                cui->mesh.text = "Exit to menu";
                entity->addComponent(cui);
        }

        {
            auto entity = new Entity(-12, 3, "Exit_game");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {2, -110};
                crt->getSizeModifiable() = {250.0f, 100.0f};
                crt->getLocalRotationModifiable() = glm::radians(-0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto ui = new UiRenderer();
                auto buttMaterial = om.newMaterial(uiStandard, "Exit_gameMat", RenderType::Transparent);
                buttMaterial->setTexture("sprite", rect_border);
                buttMaterial->setVec4("color", {1.0f, 0.0f, 1.0f, 0.5f});
                ui->material = buttMaterial;
                entity->addComponent(ui);
            
            auto butt = new Button();
                butt->baseColor = baseColor;
                butt->inactiveColor = inactiveColor;
                butt->highlightedColor = highlightedColor;
                butt->onClickColor = onClickColor;
                butt->onClickEvents.push_back(Message(Event::EXIT_GAME));
                entity->addComponent(butt);
        }

        {
            auto entity = new Entity(-13, 2, "Exit_game_text");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {-88.0f, -119.0f};
                crt->getSizeModifiable() = {0.25f, 0.25f};
                crt->getLocalRotationModifiable() = glm::radians(-0.8f);
                crt->setParent(rt);
                entity->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = textMaterial;
                cui->mesh.font = courier;
                cui->mesh.text = "Exit Game";
                entity->addComponent(cui);
        }
    }  
}

void GamePlayModule::initSummaryScreen(ObjectModule& om)
{
    summaryScreenEntity = new Entity(-100, 2, "summary");
    auto tex = om.newTexture("Resources/Sprites/summary.png", info);
    courierBold = GetCore().objectModule.newFont("Resources/Fonts/CourierNew-Bold.ttf", 128, "CourierNew-Bold");
    auto rect = new RectTransform();
    rect->getAnchorModifiable() = {0.5f, 0.5f};
    rect->getSizeModifiable() = {550, 700};
    summaryScreenEntity->addComponent(rect);

    auto uiRenderer = new UiRenderer();
    uiRenderer->material = om.newMaterial(uiStandard, "SummaryMat", RenderType::Opaque);
    uiRenderer->material->setVec4("color", {1.0f, 1.0f, 1.0f, 1.0f});
    uiRenderer->material->setTexture("sprite", tex);
    summaryScreenEntity->addComponent(uiRenderer);

    textBlackMaterial = om.newMaterial(textShader, "textBlackMat", RenderType::Transparent);
    textBlackMaterial->setVec4("color", {0.0f, 0.0f, 0.0f, 1.0f});

    //Buttons
    {
        auto rect_border = om.newTexture("Resources/Sprites/rect_border.png", info);
        glm::vec4 baseColor = {0.733f, 0.066f, 0.066f, 1.0f};
        glm::vec4 inactiveColor = {0.733f, 0.066f, 0.066f, 1.0f};
        glm::vec4 highlightedColor = {0.858f, 0.078f, 0.078f, 1.0f};
        glm::vec4 onClickColor = {0.654f, 0.066f, 0.066f, 1.0f};

        auto textMaterial = om.newMaterial(textShader, "buttonsText", RenderType::Transparent);
        textMaterial->setVec4("color", baseColor);
        {
            auto entity = new Entity(1499, 3, "ToMenuButton");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {-127, -291};
                crt->getSizeModifiable() = {150.0f, 80.0f};
                crt->getLocalRotationModifiable() = glm::radians(1.5f);
                crt->setParent(rect);
                entity->addComponent(crt);

            auto ui = new UiRenderer();
                auto buttMaterial = om.newMaterial(uiStandard, "ToMenuButtonMat", RenderType::Transparent);
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
                butt->isActive = true;
                entity->addComponent(butt);
        }

        {
            auto entity = new Entity(1498, 2, "ToMenuButtonText");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {-196, -297};
                crt->getSizeModifiable() = {0.15f, 0.15f};
                crt->getLocalRotationModifiable() = glm::radians(1.5f);
                crt->setParent(rect);
                entity->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = textMaterial;
                cui->mesh.font = courierBold;
                cui->mesh.text = "Exit to menu";
                entity->addComponent(cui);
        }

        {
            auto entity = new Entity(1497, 3, "PlayAgain");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {88, -291};
                crt->getSizeModifiable() = {150.0f, 80.0f};
                crt->getLocalRotationModifiable() = glm::radians(-1.5f);
                crt->setParent(rect);
                entity->addComponent(crt);

            auto ui = new UiRenderer();
                auto buttMaterial = om.newMaterial(uiStandard, "PlayAgainMat", RenderType::Transparent);
                buttMaterial->setTexture("sprite", rect_border);
                buttMaterial->setVec4("color", {1.0f, 0.0f, 1.0f, 0.5f});
                ui->material = buttMaterial;
                entity->addComponent(ui);
            
            auto butt = new Button();
                butt->baseColor = baseColor;
                butt->inactiveColor = inactiveColor;
                butt->highlightedColor = highlightedColor;
                butt->onClickColor = onClickColor;
                butt->onClickEvents.push_back(Message(Event::LOAD_SCENE, Scenes::selectCargoScene));
                butt->isActive = true;
                entity->addComponent(butt);
        }

        {
            auto entity = new Entity(1496, 2, "PlayAgainText");
            auto crt = new RectTransform();
                crt->getLocalPositionModifiable() = {30, -297};
                crt->getSizeModifiable() = {0.15f, 0.15f};
                crt->getLocalRotationModifiable() = glm::radians(-1.5f);
                crt->setParent(rect);
                entity->addComponent(crt);

            auto cui = new TextRenderer();
                cui->material = textMaterial;
                cui->mesh.font = courierBold;
                cui->mesh.text = "Play Again";
                entity->addComponent(cui);
        }
    }

    textMatGreen = GetCore().objectModule.newMaterial(textShader, "textMatGreen", RenderType::Opaque);
        textMatGreen->setVec4("color", {0.082f, 0.576f, 0.043f, 1.0f});
    textMatRed = GetCore().objectModule.newMaterial(textShader, "textMatRed", RenderType::Opaque);
        textMatRed->setVec4("color", {0.658f, 0.082f, 0.062f, 1.0f});

    for(int i = 0; i < 9; ++i) //making text entities
    {
        {
            goodCargoEntities[2 * i] = new Entity(-101 - (2 * i), 2, "CargoGoodName" + std::to_string(i));
            auto rect = new RectTransform();
            rect->setParent(summaryScreenEntity->getComponentPtr<RectTransform>());
            rect->getLocalPositionModifiable() = {-520, -160 + (-20 * i)};
            rect->getAnchorModifiable() = {0.5f, 0.5f};
            rect->getSizeModifiable() = {0.15f, 0.15f};
            goodCargoEntities[2 * i]->addComponent(rect);

            auto text = new TextRenderer();
            text->mesh.font = courier;
            text->material = textBlackMaterial;
            text->mesh.text = "";
            goodCargoEntities[2 * i]->addComponent(text);
        }

        {
            goodCargoEntities[2 * i + 1] = new Entity(-102 - (2 * i), 2, "CargoGoodCost" + std::to_string(i));
            auto rect = new RectTransform();
            rect->setParent(summaryScreenEntity->getComponentPtr<RectTransform>());
            rect->getLocalPositionModifiable() = {-125, -160 + (-20 * i)};
            rect->getAnchorModifiable() = {0.5f, 0.5f};
            rect->getSizeModifiable() = {0.15f, 0.15f};
            goodCargoEntities[2 * i + 1]->addComponent(rect);

            auto text = new TextRenderer();
            text->mesh.font = courierBold;
            text->mesh.text = "";
            text->material = textMatGreen;
            goodCargoEntities[2 * i + 1]->addComponent(text);
        }

        {
            badCargoEntities[2 * i] = new Entity(-120 - (2 * i), 2, "CargoBadName" + std::to_string(i));
            auto rect = new RectTransform();
            rect->setParent(summaryScreenEntity->getComponentPtr<RectTransform>());
            rect->getLocalPositionModifiable() = {-520, -375 + (-20 * i)};
            rect->getAnchorModifiable() = {0.5f, 0.5f};
            rect->getSizeModifiable() = {0.15f, 0.15f};
            badCargoEntities[2 * i]->addComponent(rect);

            auto text = new TextRenderer();
            text->mesh.font = courier;
            text->mesh.text = "";
            text->material = textBlackMaterial;
            badCargoEntities[2 * i]->addComponent(text);
        }

        {
            badCargoEntities[2 * i + 1] = new Entity(-121 - (2 * i), 2, "CargoBadCost" + std::to_string(i));
            auto rect = new RectTransform();
            rect->setParent(summaryScreenEntity->getComponentPtr<RectTransform>());
            rect->getLocalPositionModifiable() = {-125, -375 + (-20 * i)};
            rect->getAnchorModifiable() = {0.5f, 0.5f};
            rect->getSizeModifiable() = {0.15f, 0.15f};
            badCargoEntities[2 * i + 1]->addComponent(rect);

            auto text = new TextRenderer();
            text->mesh.font = courierBold;
            text->mesh.text = "";
            text->material = textBlackMaterial;
            badCargoEntities[2 * i + 1]->addComponent(text);
        }
    }

    {
        totalSumEntity = new Entity(-150, 2, "TotalText");
        auto rect = new RectTransform();
        rect->setParent(summaryScreenEntity->getComponentPtr<RectTransform>());
        rect->getLocalPositionModifiable() = {-125, -570};
        rect->getAnchorModifiable() = {0.5f, 0.5f};
        rect->getSizeModifiable() = {0.15f, 0.15f};
        totalSumEntity->addComponent(rect);

        auto text = new TextRenderer();
        text->mesh.font = courierBold;
        text->mesh.text = "";
        text->material = textBlackMaterial;
        totalSumEntity->addComponent(text);
    }
}

void GamePlayModule::togglePauseState()
{
    Core::instance->gamePaused = !Core::instance->gamePaused;
    Core::instance->messageBus.sendMessage(Message(Event::AUDIO_SOURCE_PAUSE_ALL_PLAYING));
}

void GamePlayModule::pauseScreen()
{
    useScreen(pauseScreenEntity, GetCore().gamePaused);
}   

void GamePlayModule::youDied()
{
    useScreen(youDiedScreenEntity, GetCore().gamePaused);
}

void GamePlayModule::summarize()
{
    if(GetCore().gamePaused)
    {
        // ? setup all texts
        auto storageEntity = GetCore().objectModule.getEntityPtrByName("Final_physics_meshes.FBX/Physics_kayak");
        CargoStorage* cargoStorage = storageEntity->getComponentPtr<CargoStorage>();
        if((storageEntity != nullptr) && (cargoStorage != nullptr))
        {
            float sum = 0.0f;
            
            int i = 0;
            for(auto iter = cargoStorage->cargosStored.begin(); iter != cargoStorage->cargosStored.end(); ++iter)
            {
                goodCargoEntities[i *2]->getComponentPtr<TextRenderer>()->mesh.text = (*iter)->name;
                goodCargoEntities[i * 2 + 1]->getComponentPtr<TextRenderer>()->mesh.text = getFloatWithPrecision((*iter)->income, 2) + "$";
                sum += (*iter)->income;
                ++i;
            }

            i = 0;
            for(auto iter = cargoStorage->cargosLost.begin(); iter != cargoStorage->cargosLost.end(); ++iter)
            {
                badCargoEntities[2 * i]->getComponentPtr<TextRenderer>()->mesh.text = (*iter)->name;
                auto text = badCargoEntities[2 * i + 1]->getComponentPtr<TextRenderer>();
                text->mesh.font = courierBold;
                if((*iter)->isRisky)
                {
                    text->mesh.text = std::string("-") + getFloatWithPrecision((*iter)->income * 0.2f, 2) + "$";
                    sum -= (*iter)->income * 0.2f;
                    text->material = textMatRed;
                }
                else
                {
                    text->mesh.text = "0$";
                    text->material = textBlackMaterial;
                }
                ++i;
            }

            totalSumEntity->getComponentPtr<TextRenderer>()->mesh.text = getFloatWithPrecision(sum, 2) + "$";
            totalSumEntity->getComponentPtr<TextRenderer>()->material = sum > 0.01f ? textMatGreen : textMatRed;
        }
    }
    
    useScreen(summaryScreenEntity, GetCore().gamePaused);
}

void GamePlayModule::rocksHit()
{
    auto kayak = Kayak::get();

    auto* rigidbody = kayak->entityPtr->getComponentPtr<Rigidbody>();

    float speed = glm::length(rigidbody->velocity);

    float baseDamage = speed - kayak->hitDamageTreshold;

    if (baseDamage > 0)
    {
        kayak->hp -= kayak->hitDamagefactor * baseDamage;
        kayak->hp = std::max(kayak->hp, 0.0f);

        if (healthbarPtr)
        {
            healthbarPtr->percentage = kayak->hp / kayak->maxHp;
        }

        if (kayak->hp <= 0)
        {
            GetCore().messageBus.sendMessage(Message(Event::HP_0));
        }
    }

    if (speed > kayak->hitLostCargoTreshold && GetCore().randomFloat01L() < kayak->chanceToLostPackage)
    {
        GetCore().messageBus.sendMessage(Message(Event::CARGO_LOST));
    }
}

void GamePlayModule::bulletHit()
{
    auto kayak = Kayak::get();
    if(kayak != nullptr)
    {
        kayak->hp -= bulletDamage;
        
        if (healthbarPtr)
        {
            healthbarPtr->percentage = kayak->hp / kayak->maxHp;
        }

        if (kayak->hp <= 0)
        {
            GetCore().messageBus.sendMessage(Message(Event::HP_0));
        }
    }
}

void GamePlayModule::useScreen(Entity* screenEntity, bool shown)
{
    if(screenEntity != nullptr)
    {
        if(shown)
        {
            //? add parent entity
            GetCore().objectModule.objectContainer.entities.push_back(*screenEntity);
            GetCore().uiModule.rootNodes.push_back(screenEntity->getComponentPtr<RectTransform>());
            //? add children according to transform
            if(screenEntity->getComponentPtr<RectTransform>() != nullptr)
            {
                for(auto pChildren : screenEntity->getComponentPtr<RectTransform>()->children)
                {
                    GetCore().objectModule.objectContainer.entities.push_back(*pChildren->entityPtr);
                }
            }
        }
        else
        {
            //? remove parent entity
            auto iter2 = std::find(GetCore().objectModule.objectContainer.entities.begin(), GetCore().objectModule.objectContainer.entities.end(), *screenEntity);
            if(iter2 != GetCore().objectModule.objectContainer.entities.end())
            {
                GetCore().objectModule.objectContainer.entities.erase(iter2);
            }
            //? remove children according to transform
            if(screenEntity->getComponentPtr<RectTransform>() != nullptr)
            {
                for(auto pChildren : screenEntity->getComponentPtr<RectTransform>()->children)
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