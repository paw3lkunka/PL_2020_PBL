#ifndef SORTINGGROUPSYSTEM_HPP_
#define SORTINGGROUPSYSTEM_HPP_

#include "System.hpp"

struct UiSortingGroup;
struct RectTransform;

/**
 * @brief system of transparency sorting groups
 */
class SortingGroupSystem : public System
{
public:
    SortingGroupSystem() = default;
    ~SortingGroupSystem() = default;

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
        virtual void start();

        /**
         * @brief Contain logic of the system called in fixed time steps
         * called in Process() only when AssertEntity() returned true
         * should use class variables to access components
         */
        virtual void fixedUpdate() {}

        /**
         * @brief Contain logic of the system runned once per frame
         * called in Process() only when AssertEntity() returned true
         * should use class variables to access components
         */
        virtual void frameUpdate();
private:
    UiSortingGroup* sortingGroupPtr;
    RectTransform* rectTransformPtr;

    void setChildrenTransparency(RectTransform* parent);
};

#endif /* !SORTINGGROUPSYSTEM_HPP_ */
