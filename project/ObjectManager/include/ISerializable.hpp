#ifndef ISERIALIZABLE_INL_
#define ISERIALIZABLE_INL_

class ObjectModule;

class ISerializable
{
    friend class ObjectModule;
public:
    ISerializable(bool makeSerializable = true) 
    {
        serialize = makeSerializable;
        if (makeSerializable)
        {
            serializationID = nextID++;
        }
    }
    unsigned int serializationID;
    bool serialize;

    virtual ~ISerializable() = default;
private:
    static unsigned int nextID;
};

#endif /* !ISERIALIZABLE_INL_ */
