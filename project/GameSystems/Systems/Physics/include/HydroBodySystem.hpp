#ifndef _HYDROBODYSYSTEM_HPP
#define _HYDROBODYSYSTEM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

#include "System.hpp"

class Entity;

struct HydroBody;
struct HydroAccelerator;
struct Transform;
struct Rigidbody;
struct MeshRenderer;
struct HydroVertexData;
struct HydroTriangleData;

class HydroBodySystem : public System
{
    public:
        HydroBodySystem() = default;
        virtual ~HydroBodySystem() = default;
        
        virtual bool assertEntity(Entity* entity);
        virtual void fixedUpdate();

    protected:
    private:
        HydroBody* hydroBody;
        HydroAccelerator* hydroAccelerator;
        Transform* transform;
        Rigidbody* rigidbody;
        MeshRenderer* meshRenderer;

        glm::vec3 modelTranslation;
        glm::vec3 modelVelocity;
        glm::vec3 modelAngularVelocity;

        // TODO: Modify and document these
        std::vector<unsigned int> indexOfOriginalTriangle;
        std::vector<HydroTriangleData> underSurfaceTriangleData;
        std::vector<HydroTriangleData> aboveSurfaceTriangleData;

        std::vector<glm::vec3> bodyVerticesGlobal;
        std::vector<float> bodyDistancesToSurface;

        std::vector<glm::vec3> intersectionVertices;

        float timeSinceStart;
        float bodyArea;

        float minSubmergedZ;
        float maxSubmergedZ;

        /// @brief Maximal witnessed acceleration during the fall
        float slam_acc_max;

        /// @brief Add all forces that act on the submerged polygons
        void addUnderSurfaceForces();

        /// @brief Add all forces that act on the non-submerged polygons
        void addAboveSurfaceForces();

        /// @brief Calculate new velocities for slamming force purposes
        void calculateSlammingVelocities();

        /// @brief Collect all triangles under and above surface of the fluid and other needed values
        void generateUnderSurfaceMesh();

        /// @brief Process each triangle to tell if it is submerged and how 
        void addTriangles();

        /// @brief Process a triangle with only one vertex submerged
        void addTrianglesOneVertexAboveSurface(std::vector<HydroVertexData> vertexData, int triangleCounter);
        
        /// @brief Process a triangle with two vertices submerged
        void addTrianglesTwoVerticesAboveSurface(std::vector<HydroVertexData> vertexData, int triangleCounter);
        
        /// @brief Calculate submerged area
        void calculateSubmergedTrianglesArea();

        /// @brief Checks if triangle has a minimal/maximal Z submerged value
        void checkSubmergedMinMaxZ(HydroTriangleData triangle);
};

#endif // _HYDROBODYSYSTEM_HPP
