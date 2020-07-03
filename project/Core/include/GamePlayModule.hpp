#ifndef GAMEPLAYMODULE_HPP_
#define GAMEPLAYMODULE_HPP_

#include "IModule.inl"
#include <string>
#include "Texture.hpp"

class Entity;
class Shader;
struct ProgressBar;
class Font;
class Material;
struct Tutorial;

class GamePlayModule : public IModule
{
public:
    GamePlayModule() = default;
    ~GamePlayModule() = default;

    ///@brief inherited from IModule
    virtual void receiveMessage(Message msg);

    ///@brief initialize module
    void init(const char* hpBarName, float bulletDamage);

    ///@brief initialize screens
    void initScreens();

protected:
private:
    Entity* loadingScreenEntity = nullptr;
    Entity* pauseScreenEntity = nullptr;
    Entity* youDiedScreenEntity = nullptr;
    Entity* summaryScreenEntity = nullptr;

    ///@brief array of entity pointers - used for summary- 9 cargos with 2 texts
    Entity* goodCargoEntities[18] = {nullptr};
    Entity* badCargoEntities[18] = {nullptr};
    Entity* totalSumEntity = nullptr;

    Shader* uiStandard = nullptr;
    Shader* textShader = nullptr;

    Material* textBlackMaterial = nullptr;
    Material* textMatGreen = nullptr;
    Material* textMatRed = nullptr;

    Font* courier = nullptr;
    Font* josefin = nullptr;
    Font* courierBold = nullptr;

    Tutorial* tutorialComponent = nullptr;
    bool showTutorial = true;

    TextureCreateInfo info = {};

    void reloadScene(std::string name);

    void initLoadingScreen(ObjectModule& om);
    void initPauseScreen(ObjectModule& om);
    void initYouDieScreen(ObjectModule& om);
    void initSummaryScreen(ObjectModule& om);

    ///@brief toggle pause mode
    void togglePauseState();
    ///@brief show pause screen
    void pauseScreen();
    ///@brief show you died screen
    void youDied();
    ///@brief summarize game
    void summarize();

    /**
     * @brief show/ hide ui screen method
     * 
     * @param screenEntity entity to show
     * @param shown should screen be shown flag
     */
    void useScreen(Entity* screenEntity, bool shown);

    /**
     * @brief Called, when kayak hit obsticles.
     * Also sets health bar - if present.
     */
    void rocksHit();

    /**
     * @brief Called, when kayak is hitten by bullet.
     * Also sets health bar - if present.
     */
    void bulletHit();
    
    ProgressBar* healthbarPtr = nullptr;
    float bulletDamage = 100.0f;
};

#endif /* !GAMEPLAYMODULE_HPP_ */
