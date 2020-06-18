#ifndef GAMEPLAYMODULE_HPP_
#define GAMEPLAYMODULE_HPP_

#include "IModule.inl"
#include <string>
#include "Texture.hpp"

class Entity;
class Shader;
struct ProgressBar;

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

    Shader* uiStandard;

    TextureCreateInfo info = {};

    void reloadScene(std::string name);

    void initLoadingScreen(ObjectModule& om);
    void initPauseScreen(ObjectModule& om);
    void initYouDieScreen(ObjectModule& om);

    ///@brief show pause screen
    void pauseGame();
    ///@brief show you died screen
    void youDied();

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
