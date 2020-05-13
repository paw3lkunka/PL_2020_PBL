#ifndef COLLISIONDATASTRUCTURES_INL_
#define COLLISIONDATASTRUCTURES_INL_

#include <functional>

#include <glm/glm.hpp>

#include "Collider.inl"

/**
 * @brief Data structure for collision and trigger events
 * 
 */
struct CollisionData
{
    /// @brief Collider which interacted with other
    Collider* cause;
    /// @brief Collider which was entered
    Collider* target;
};

/**
 * @brief Stores object projected on straight line 
 */
struct Projection1D
{
    float start;
    float end;
};

inline bool operator == (CollisionData a, CollisionData b) { return a.cause == b.cause && a.target == b.target; }

class CollisionDataHasher
{
    public:
        size_t operator () (const CollisionData& td) const
        {
            return (std::hash<Collider*>()(td.cause) + 1147499) ^ (std::hash<Collider*>()(td.target) + 2000029);
        }
};

class CollisionDataEquals
{
    public:
        bool operator () (const CollisionData& t1, const CollisionData& t2) const
        {
            return t1 == t2;
        }
};


#endif /* !COLLISIONDATASTRUCTURES_INL_ */