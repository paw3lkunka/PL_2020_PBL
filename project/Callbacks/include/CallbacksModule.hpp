#ifndef CALLBACKS_HPP_
#define CALLBACKS_HPP_

#include "IModule.inl"

class ProgressBar;

/**
 * @brief Class containing various unconnected callbacks for Messages. 
 */
class CallbacksModule : public IModule
{
    public:
        CallbacksModule() = default;
        virtual ~CallbacksModule() = default;
        
        /**
         * @brief Set initial values, necessery to woking properly.
         * 
         * @param hpBarName name of health bar entity.
         * TODO - may use create data, when list of parameters will be to long
         */
        void init(const char* hpBarName);

        virtual void receiveMessage(Message msg);
        
    private:
        /**
         * @brief Called, when kayak hit obsticles.
         * Also sets health bar - if present.
         */
        void rocksHit();
        
        ProgressBar* healthbarPtr = nullptr;
};

#endif /* !CALLBACKS_HPP_ */

