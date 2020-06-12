#ifndef _MESHRENDERER_INL
#define _MESHRENDERER_INL

#include "Component.hpp"

#include <glm/glm.hpp>

class Material;
class Mesh;

/**
 * @brief Mesh rendering component
 */
struct MeshRenderer : public Component
{
    MeshRenderer() = default;
    virtual ~MeshRenderer() = default;

    /**
     * @brief The model matrix of this mesh
     */
    glm::mat4 modelMatrix;

    /// @brief Pointer to mesh's material
    Material* material;
    /// @brief Pointer to mesh to be rendered
    Mesh* mesh;
};

#endif // _MESHRENDERER_INL
