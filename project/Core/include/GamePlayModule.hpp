#ifndef GAMEPLAYMODULE_HPP_
#define GAMEPLAYMODULE_HPP_

#include "IModule.inl"
#include <string>

class Entity;

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

    void reloadScene(std::string name);

    
};

#endif /* !GAMEPLAYMODULE_HPP_ */
