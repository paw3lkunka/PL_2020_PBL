#ifndef ISERIALIZABLE_INL_
#define ISERIALIZABLE_INL_

class ISerializable
{
public:
    ISerializable() 
    {
        static unsigned int nextID = 0;
        serializationID = nextID++;
    }
    unsigned int serializationID;

    virtual ~ISerializable() = default;
    
};

#endif /* !ISERIALIZABLE_INL_ */
