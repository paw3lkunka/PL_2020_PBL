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

/**
 * @brief Data structure storeing all results of SAT test.
 */
struct SATTestResult
{
    /**
     * @brief Inform if collision was detected (SAT test passed).
     */
    bool collisionDetected;
    /**
     * @brief Center of colliders intersection, depending on colliders final type. 
     */
    glm::vec3 collisionCentre;
    /**
     * @brief Normal vector of collision, determine reaction force direction.  
     */
    glm::vec3 collisionNormal;
    /**
     * @brief Width of colliders intersection in axis of collision normal.
     * Penetration vector can be calculates as collisionNormal * penetration.
     */
    float penetration;
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