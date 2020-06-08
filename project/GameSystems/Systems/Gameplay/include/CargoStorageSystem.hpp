#ifndef CARGOSTORAGESYSTEM_HPP_
#define CARGOSTORAGESYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"
#include <iterator>
#include <list>

struct CargoStorage;
struct Cargo;

class CargoStorageSystem : public System, public IMsgReceiver
{
public:
    CargoStorageSystem() = default;
    ~CargoStorageSystem() = default;

    ///@brief inherited from IMsgReceiver
    void receiveMessage(Message msg);

    /**
     * @brief Checks, if given Entity contain all required components
     * implementation should also save pointers to components in class variables 
     * @param entity pointer to asserted entity
     * @returns if entity has all required components
     */
    virtual bool assertEntity(Entity* entity);

    /**
     * @brief Initializes processed entities, runs before first update
     */
    virtual void start() {};

    /**
     * @brief Contain logic of the system called in fixed time steps
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    virtual void fixedUpdate() {};

    /**
     * @brief Contain logic of the system runned once per frame
     * called in Process() only when AssertEntity() returned true
     * should use class variables to access components
     */
    virtual void frameUpdate();

protected:
private:
    CargoStorage* cargoStoragePtr = nullptr;

    /**
     * @brief find cargo in list method
     * 
     * @param vec storage where it will be looking in
     * @param toFind cargo to find in storage
     * @return true cargo found
     * @return false cargo not found
     */
    std::list<Cargo*>::iterator findCargo(std::list<Cargo*> vec, Cargo* toFind);
};

#endif /* !CARGOSTORAGESYSTEM_HPP_ */
