#ifndef GAMEPLAYMODULE_HPP_
#define GAMEPLAYMODULE_HPP_

#include "IModule.inl"
#include <string>
#include "Texture.hpp"

class Entity;
class Shader;

class GamePlayModule : public IModule
{
public:
    GamePlayModule() = default;
    ~GamePlayModule() = default;

    ///@brief inherited from IModule
    virtual void receiveMessage(Message msg);

    ///@brief initialize module
    void init();

protected:
private:
    Entity* loadingScreenEntity;
    Entity* pauseScreenEntity;

    Shader* uiStandard;

    TextureCreateInfo info = {};

    void reloadScene(std::string name);

    void initLoadingScreen(ObjectModule& om);
    void initPauseScreen(ObjectModule& om);
};

#endif /* !GAMEPLAYMODULE_HPP_ */
