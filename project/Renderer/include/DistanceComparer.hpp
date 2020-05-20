#ifndef DISTANCECOMPARER_HPP_
#define DISTANCECOMPARER_HPP_

#include "NormalPacket.inl"
#include <glm/glm.hpp>

class DistanceComparer
{
    public:
        DistanceComparer(glm::vec3 cameraPos);
        virtual ~DistanceComparer() = default;

        bool operator() (NormalPacket* packet1, NormalPacket* packet2);
    protected:
    private:
        glm::vec3 cameraPos;

};

#endif /* !DISTANCECOMPARER_HPP_ */
