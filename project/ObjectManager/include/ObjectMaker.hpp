#ifndef OBJECTMAKER_HPP_
#define OBJECTMAKER_HPP_

class ObjectModule;

class ObjectMaker
{
public:
    ObjectMaker(ObjectModule* objectmodulePtr);
    ~ObjectMaker() = default;

protected:
private:
    ObjectModule* objModPtr;
};

#endif /* !OBJECTMAKER_HPP_ */
