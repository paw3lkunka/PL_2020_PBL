#ifndef GAMEPLAYMODULE_HPP_
#define GAMEPLAYMODULE_HPP_

#include "IModule.inl"

class GamePlayModule : public IModule
{
public:
    GamePlayModule() = default;
    ~GamePlayModule() = default;

    ///@brief inherited from IModule
    virtual void receiveMessage(Message msg);

protected:
private:
};

#endif /* !GAMEPLAYMODULE_HPP_ */
