#ifndef GAMESYSTEMS_HPP_
#define GAMESYSTEMS_HPP_

#include "IModule.inl"

/**
 * @brief Module responsible for running ECS systems * 
 */
class GameSystemsModule : public IModule 
{
    public:        
        /**
         * @brief IModule implementation, temporally does nothing
         * 
         * @param msg - Messange to handle
         */
        virtual void ReceiveMessage(Message msg);
    protected:
    private:
};

#endif /* !GAMESYSTEMS_HPP_ */
