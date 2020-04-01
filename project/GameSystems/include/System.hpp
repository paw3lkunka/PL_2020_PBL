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
         * @brief Updates state of entity
         * THIS FUNCTION SHOULDN'T BE OVERWRITTED IN ANY CASE!
         * @param entity 
         */
        void Process(Entity* entity);

    protected:
        /**
         * @brief Checks, if given Entity contain all required components
         * implementation should also save pointers to components in class variables 
         * @param entity pointer to asserted entity
         * @returns if entity has all required components
         */
        virtual bool AssertEntity(Entity* entity) = 0;

        /**
         * @brief Contain logic of the system
         * called in Process() only when AssertEntity() returned true
         * should use class variables to access components
         */
        virtual void Update() = 0;
        
        virtual ~System() = default;
    private:
        //in implementation here should be pointers to required components
};

#endif /* !SYSTEM_HPP_ */
