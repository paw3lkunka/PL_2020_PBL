#ifndef ISERIALIZABLE_INL_
#define ISERIALIZABLE_INL_

class ISerializable
{
public:
    ISerializable(bool makeSerializable = true) 
    {
        static unsigned int nextID = 0;
        if (makeSerializable)
        {
            serializationID = nextID++;
        }
    }
    unsigned int serializationID;

    virtual ~ISerializable() = default;
    
};

#endif /* !ISERIALIZABLE_INL_ */
