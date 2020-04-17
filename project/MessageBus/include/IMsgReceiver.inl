#ifndef I_MSG_RECEIVER_INL
#define I_MSG_RECEIVER_INL

class Message;

/**
 * @brief Module interface, allowing communication via MessageBus 
 */
class IMsgReceiver 
{
    public:
        virtual void receiveMessage(Message msg) = 0;
};

#endif /* !I_MSG_RECEIVER_INL */
