#ifndef _HYDROMESHPARSER_HPP
#define _HYDROMESHPARSER_HPP

#include <vector>
#include <glm/glm.hpp>

struct HydroTriangle;
class Mesh;

class HydroMeshParser
{
public:
    HydroMeshParser() = default;
    virtual ~HydroMeshParser() = default;

    std::vector<HydroTriangle> parse(Mesh mesh, glm::mat4 modelMatrix);
    glm::vec3 vertexToModel(glm::vec3, glm::mat4 modelMatrix);

private:
    std::vector<float> verticesHeights;
};


#endif // _HYDROMESHPARSER_HPP
