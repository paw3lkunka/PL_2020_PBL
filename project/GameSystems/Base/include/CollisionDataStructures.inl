#ifndef COLLISIONDATASTRUCTURES_INL_
#define COLLISIONDATASTRUCTURES_INL_

#include <functional>

#include <glm/glm.hpp>

#include "Collider.inl"

/**
 * @brief Data structure for collision event
 * 
 */
struct __attribute__((packed)) CollisionData
{
    /// @brief Collider for which collision was resolved
    Collider* cause;
    /// @brief Collider with which collision was resolved
    Collider* target;
    //TODO should be removed / changed after phisic collisions implementation
    /// @brief Separation vector for 'cause' collider
    glm::vec3 separation;
};

/**
 * @brief Data structure for trigger events
 * 
 */
struct TriggerData
{
    /// @brief Collider which interacted with trigger
    Collider* cause;
    /// @brief Trigger
    Collider* trigger;
};

/**
 * @brief Stores object projected on straight line 
 */
struct Projection1D
{
    float start;
    float end;
};

inline bool operator == (TriggerData a, TriggerData b) { return a.cause == b.cause && a.trigger == b.trigger; }

class TriggerDataHasher
{
    public:
        size_t operator () (const TriggerData& td) const
        {
            return (std::hash<Collider*>()(td.cause) + 1147499) ^ (std::hash<Collider*>()(td.trigger) + 2000029);
        }
};

class TriggerDataEquals
{
    public:
        bool operator () (const TriggerData& t1, const TriggerData& t2) const
        {
            return t1 == t2;
        }
};


#endif /* !COLLISIONDATASTRUCTURES_INL_ */