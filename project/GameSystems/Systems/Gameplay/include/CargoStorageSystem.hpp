#ifndef CARGOSTORAGESYSTEM_HPP_
#define CARGOSTORAGESYSTEM_HPP_

#include "System.hpp"
#include "IMsgReceiver.inl"
#include <iterator>
#include <vector>

struct CargoStorage;
struct Cargo;
struct TextRenderer;
struct UiSortingGroup;


class CargoStorageSystem : public System, public IMsgReceiver
{
public:
    typedef std::vector<Cargo*>::iterator Iterator;
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
    virtual bool assertEntity(Entity* entity) {return false;}

    /**
     * @brief initialize system
     * 
     * @param cs pointer for CargoStorage
     */
    void init(CargoStorage* cs);

    /**
     * @brief initialize some texts
     * 
     * @param weightText text for weights
     * @param incomeText text for income
     */
    void initTexts(TextRenderer* weightText, TextRenderer* incomeText, UiSortingGroup* warningGroup);

    /**
     * @brief Initializes processed entities, runs before first update
     */
    virtual void start() {}

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
    virtual void frameUpdate() {}

protected:
private:
    void addCargo(Cargo* cargoPtr);
    void removeCargo(Iterator cargoIter);
    void LooseCargo(Iterator cargoIter);
    void updateButtonsState(Cargo* cargoPtr);

    CargoStorage* cargoStoragePtr = nullptr;
    TextRenderer* weightText = nullptr;
    TextRenderer* incomeText = nullptr;
    UiSortingGroup* warningGroup = nullptr;
};

#endif /* !CARGOSTORAGESYSTEM_HPP_ */
