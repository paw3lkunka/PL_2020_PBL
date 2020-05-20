#ifndef COLLISION_SYSTEM_HELPER_FUNCTIONS
#define COLLISION_SYSTEM_HELPER_FUNCTIONS

#include <glm/glm.hpp>

#include "Rigidbody.inl"

class BoxCollider;
class Transform;
class SphereCollider;
class Projection1D;

/**
 * @brief Project box collider to 1D range in space of given line.
 * 
 * @param box box to project.
 * @param transform transform of projected box.
 * @param axisPoint1 first point defining aline.
 * @param axisPoint2 second point defining aline.
 * @param projBuffer pointer to two element vec3 array, where coordinates of beginnign and end point of projection will be saved.
 * @return Projection1D 1D projection of box in line space.
 */
Projection1D AxisProjection(BoxCollider* box, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2, glm::vec3 projBuffer[]);

/**
 * @brief Project sphere collider to 1D range in space of given line.
 * 
 * @param sphere sphere to project.
 * @param transform transform of projected sphere.
 * @param axisPoint1 first point defining aline.
 * @param axisPoint2 second point defining aline.
 * @param projBuffer pointer to two element vec3 array, where coordinates of beginnign and end point of projection will be saved.
 * @return Projection1D 1D projection of sphere in line space.
 */
Projection1D AxisProjection(SphereCollider* sphere, Transform* transform, glm::vec3 axisPoint1, glm::vec3 axisPoint2, glm::vec3 projBuffer[]);

/**
 * @brief Project point onto given line.
 * 
 * @param point point to project.
 * @param axisPoint1 first point defining aline.
 * @param axisPoint2 second point defining aline.
 * @return glm::vec3 projection of point on line.
 */
glm::vec3 AxisProjection(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2);

/**
 * @brief Transform 3D point to 1D line space coordinate.
 * 
 * @param point point to transform.
 * @param axisPoint1 first point defining aline.
 * @param axisPoint2 second point defining aline.
 * @return float coordinate in line space.
 */
float ToLineSpace1D(glm::vec3 point, glm::vec3 axisPoint1, glm::vec3 axisPoint2);

//http://jonathanbosson.github.io/reports/TNM085_group5.pdf

/**
 * @brief Return relative velocity between the two colliding points of the object.
 * 
 * @param rBody1 first colliding object
 * @param rBody2 second colliding object
 * @param r1 vector from first object centre to collision point
 * @param r2 vector from first object centre to collision point
 * @param collisionNormal normal vector of collision
 * @return glm::vec3 relative velocity between the two colliding points
 */
glm::vec3 RelativeVelocity(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal);
/**
 * @brief Calculate j Impuls, details here: http://jonathanbosson.github.io/reports/TNM085_group5.pdf
 * 
 * @param rBody1 first colliding object
 * @param rBody2 second colliding object
 * @param r1 vector from first object centre to collision point
 * @param r2 vector from first object centre to collision point
 * @param collisionNormal normal vector of collision
 * @return glm::vec3 relative velocity between the two colliding points
 */
glm::vec3 JImpulse(Rigidbody* rBody1, Rigidbody* rBody2, const glm::vec3& r1, const glm::vec3& r2, const glm::vec3& collisionNormal);

#endif