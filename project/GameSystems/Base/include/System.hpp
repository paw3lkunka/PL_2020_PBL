#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

class Entity;

/**
 * @brief Base class for all ECS systems
 * implementation should have private pointers to required Components
 */
class System
{
    public:
        /**
         * @brief identifiers for types of update. 
         */
        enum UpdateType
        {
            START,
            FIXED,
            FRAME
        };

        /**
         * @brief Updates state of entity
         * THIS FUNCTION SHOULDN'T BE OVERWRITTED IN ANY CASE!
         * @param entity - entity to process
         * @param entity - type of update
         */
        void process(Entity* entity, UpdateType updateType);

    protected:
        /**
         * @brief Checks, if given Entity contain all required components
         * implementation should also save pointers to components in class variables 
         * @param entity pointer to asserted entity
         * @returns if entity has all required components
         */
        virtual bool assertEntity(Entity* entity) = 0;

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
        virtual void frameUpdate() {};
        
        virtual ~System() = default;
    private:
        //in implementation here should be pointers to required components
};

#endif /* !SYSTEM_HPP_ */
