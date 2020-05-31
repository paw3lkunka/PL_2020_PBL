#ifndef DISTANCECOMPARER_HPP_
#define DISTANCECOMPARER_HPP_

#include "NormalPacket.hpp"
#include <glm/glm.hpp>

/**
 * @brief Comparer used to sorting transparent queue.
 */
class DistanceComparer
{
    public:
        /**
         * @brief Construct a new Distance Comparer object.
         * 
         * @param cameraPos global position of camera.
         */
        DistanceComparer(glm::vec3 cameraPos);
        virtual ~DistanceComparer() = default;

        /**
         * @brief Compares two render packet, by distance ot their meshes to camera.
         * 
         * @param packet1 Renderer packet to compare.
         * @param packet2 Renderer packet to compare.
         * @return If packet1 is further than packet2.
         */
        bool operator() (NormalPacket* packet1, NormalPacket* packet2);
    protected:
    private:
        glm::vec3 cameraPos;

};

#endif /* !DISTANCECOMPARER_HPP_ */
