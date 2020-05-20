
#include "DistanceComparer.hpp"

DistanceComparer::DistanceComparer(glm::vec3 cameraPos)
{
    this->cameraPos = cameraPos;
}

bool DistanceComparer::operator() (NormalPacket* packet1, NormalPacket* packet2)
{
    glm::vec3 pos1 = packet1->modelMatrix[3];
    glm::vec3 pos2 = packet2->modelMatrix[3];

    return glm::length(cameraPos - pos1) > glm::length(cameraPos - pos2);
}