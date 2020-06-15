#ifndef CALLBACKS_HPP_
#define CALLBACKS_HPP_

#include "IModule.inl"

/**
 * @brief Class containing various unconnected callbacks for Messages. 
 */
class CallbacksModule : public IModule
{
    public:
        CallbacksModule() = default;
        virtual ~CallbacksModule() = default;
        
        virtual void receiveMessage(Message msg);
        
    private:
        void rocksHit();
};

#endif /* !CALLBACKS_HPP_ */

