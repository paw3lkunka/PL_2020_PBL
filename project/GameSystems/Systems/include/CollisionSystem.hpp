#ifndef COLLISIONDETECTION_HPP_
#define COLLISIONDETECTION_HPP_

#include <vector>
#include <unordered_set>

#include <glm/glm.hpp>

#include "CollisionDataStructures.inl"
#include "System.hpp"

enum class ComponentType;
struct Projection1D;
class Transform;
class Collider;
class BoxCollider;
class SphereCollider;
class Rigidbody;

/**
 * @brief System responsible for detecting and resolveing collisions.
 * Produce impulses proceed by physic system.
 */
class CollisionSystem : public System
{
public:
    CollisionSystem() = default;
        virtual ~CollisionSystem() = default;

    protected:
        virtual void start();        
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    private:    
        Collider* colliderPtr;
        Transform* transformPtr;
        Rigidbody* rigidbodyPtr;

        ///@brief Collection of colliders of all entities, which we can interact
        std::vector<Collider*> colliders;

        ///@brief Collection of transforms of all entities, which we can interact
        std::vector<Transform*> transforms;

        ///@brief Collection of rigidbodies of all entities, which we can interact
        std::vector<Rigidbody*> rigidbodies;
        
        ///@brief Collection of pairs of colliders actally intersecting triggers
        std::unordered_set<CollisionData,CollisionDataHasher,CollisionDataEquals> activeTriggers;

        /**
         * @brief Detect and resolve colision of curent entity, depending on real collider type.
         * 
         * @tparam T Final collider type.
         * @param collider Pointer to collider.
         */
        template<class T>
        void collisionOf(T* collider);

        /**
         * @brief Detect and resolve colision of two colliders, depending on them real types.
         * 
         * @tparam T1 Final type of first collider.
         * @tparam T2 Final type of second collider.
         * @param collider1 collider of currently parsed entity.
         * @param collider2 collider with which collision we want to resolve.
         * @param transform2 transform of currently parsed entity.
         * @param rigidbody2 transform with which collision we want to resolve.
         */
        template<class T1, class T2>
        void collisionWith(T1* collider1, T2* collider2, Transform* transform2, Rigidbody* rigidbody2);

        /**
         * @brief Detect colision between two entities.
         * 
         * @tparam T1 Final type of first entity's collider.
         * @tparam T2 Final type of second entity's collider.
         * @param of collider of first entity.
         * @param with collider of second entity.
         * @param ofT transform of first entity.
         * @param withT transform of second entity.
         * @return if collision was detected
         */
        template<class T1, class T2>
        bool detectCollsion(T1* of, T2* with, Transform* ofT, Transform* withT);

        /**
         * @brief Detect colision between two entities. In effect, first rigidbody gains collision impulse.
         * 
         * @tparam T1 Final type of first entity's collider.
         * @tparam T2 Final type of second entity's collider.
         * @param body1 rigidbody of second entity.
         * @param body2 rigidbody of second entity. 
         * @param transform1 transform of first entity.
         * @param transform2 transform of second entity.
         */
        template<class T1, class T2>
        void resolveCollsion(T1* collider1, T2* collider2, Rigidbody* body1, Rigidbody* body2, Transform* transform1, Transform* transform2);

        /**
         * @brief find all axes needed to perform Separate Axes Theorem Test, and push them into vector.
         * 
         * @tparam T1 Final type of first entity's collider.
         * @tparam T2 Final type of second entity's collider.
         * @param collider1 collider of first entity.
         * @param collider2 collider of second entity.
         * @param axes vector of axes defines as pairs of points - even indexes represents first points, odd - second points.
         */
        template<class T1, class T2>
        void findSATAxes(BoxCollider* , std::vector<glm::vec3>& axes);


        /**
         * @brief Perform Separate Axes Theorem Test, and returns result
         *  
         * @tparam T1 Final type of first entity's collider.
         * @tparam T2 Final type of second entity's collider.
         * @param collider1 collider of first entity.
         * @param collider2 collider of second entity.
         * @param transform1 transform of first entity.
         * @param transform2 transform of second entity.
         * @param axes vector of axes defines as pairs of points - even indexes represents first points, odd - second points.
         * @return if collision was detected
         */
        template<class T1, class T2>
        bool SATTest(T1* collider1, T2* collider2, Transform* transform1, Transform* transform2, std::vector<glm::vec3>& axes);

//TODO should it be here?

        /**
         * @brief Project box collider to 1D range in space of given line.
         * 
         * @param box box to project.
         * @param transform transform of projected box.
         * @param axisPoint1 first point defining aline.
         * @param axisPoint2 second point defining aline.
         * @return Projection1D 1D projection of box in line space.
         */
        static Projection1D axisProjection(BoxCollider* box, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2);
        
        /**
         * @brief Project sphere collider to 1D range in space of given line.
         * 
         * @param sphere sphere to project.
         * @param transform transform of projected sphere.
         * @param axisPoint1 first point defining aline.
         * @param axisPoint2 second point defining aline.
         * @return Projection1D 1D projection of sphere in line space.
         */
        static Projection1D axisProjection(SphereCollider* sphere, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2);

        /**
         * @brief Project point onto given line.
         * 
         * @param point point to project.
         * @param axisPoint1 first point defining aline.
         * @param axisPoint2 second point defining aline.
         * @return glm::vec3 projection of point on line.
         */
        static glm::vec3 axisProjection(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2);

        /**
         * @brief Transform 3D point to 1D line space coordinate.
         * 
         * @param point point to transform.
         * @param axisPoint1 first point defining aline.
         * @param axisPoint2 second point defining aline.
         * @return float coordinate in line space.
         */
        static float toLineSpace1D(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2);
};

#include "CollisionSystem.ipp"

#endif /* !COLLISIONDETECTION_HPP_ */
