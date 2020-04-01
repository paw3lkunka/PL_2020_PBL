#ifndef IMODULE_HPP_
#define IMODULE_HPP_

class Message;

/**
 * @brief Module interface, allowing communication via MessageBus 
 */
class IModule 
{
    public:
        virtual void ReceiveMessage(Message msg) = 0;
};

#endif /* !IMODULE_HPP_ */
