/**
 * @file Material.hpp
 * @author Filip Mazurek (216835@edu.p.lodz.pl)
 * @brief Material class for shader settings abstraction
 * @version 0.1
 * @date 2020-04-10
 * 
 * @copyright Copyright (c) 2020
 */
#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

#include "Texture.hpp"
#include "Cubemap.hpp"
#include "ISerializable.hpp"

#include "Shader.hpp"

enum class RenderType { Opaque, Transparent };

/**
 * @brief Class encapsulating shader program and uniform variables
 *
 * @instancing To enable instanicing for this material, set the enableInstancing flag to true.
 *   For instancing to work properly, the shader must have the attribute "instancedModel" enabled on location 5.
 *   If the attribute doesn't exist, the material will display an error and fall back to normal rendering.
 */
class Material: public ISerializable
{
friend class SceneWriter;
public:
    /**
     * @brief Construct a new Material object
     * 
     * @param shader Shader to draw with
     * @param name name of the material
     */
    Material(Shader* shader, const char* name, RenderType renderType, bool enableInstancing = false, bool serialize = true);
    ~Material() = default;

    /**
     * @brief Sets all uniforms and binds appropriate shader program
     */
    void use();

    /**
     * @brief Get whether material is instanced or not
     */
    bool isInstancingEnabled() { return enableInstancing; }

    /**
     * @brief Get the Material's Render Type 
     */
    RenderType getRenderType() { return renderType; }

    /**
     * @brief Performs matrix multiplication and sets the MVP matrix, should be used only during rendering
     * @details For performance reasons this function doesn't save the matrix but sends it immediately to the shader 
     * @important Should be used only AFTER calling use on material
     * 
     * @param M The model matrix
     * @param VP Premultiplied view and projection matrix
     * @param name Uniform name, "MVP" by default
     */
    void setMVP(const glm::mat4& M, const glm::mat4& VP, std::string name = "MVP");
    /**
     * @brief Sets the model uniform in the shader once, should only be used during rendering.
     * @details Usually passed only for use with special shader effects, otherwise use setMVP for performance
     * @important Should be used only AFTER calling use on material
     * 
     * @param M The model matrix
     * @param name Uniform name, "model" by default
     */
    void setModel(const glm::mat4& M, std::string name = "model");

    /// @brief Set texture of given name in material
    void setTexture(std::string name, Texture* value);
    /// @brief Set texture of given name in material
    void setCubemap(std::string name, Cubemap* value);
    /// @brief Set int of given name in material
    void setInt(std::string name, int value);
    /// @brief Set float of given name in material
    void setFloat(std::string name, float value);
    /// @brief Set vec3 of given name in material
    void setVec3(std::string name, glm::vec3 value);
    /// @brief Set vec4 of given name in material
    void setVec4(std::string name, glm::vec4 value);
    /// @brief Set mat4 of given name in material
    void setMat4(std::string name, glm::mat4 value);

    /// @brief Get pointer to underlying shader
    Shader* getShaderPtr();

    /**
     * @brief Get the Name of the material
     * 
     * @return const char* name
     */
    const char* getName();
    
    /**
     * @brief Get the id of material
     * 
     * @return unsigned int Material's ID
     */
    unsigned int getID() { return ID; }

    /// @brief Get int of given name
    int getInt(std::string name);
    /// @brief Get float of given name
    float& getFloat(std::string name);
    /// @brief Get vec3 of given name
    glm::vec3& getVec3(std::string name);
    /// @brief Get vec4 of given name
    glm::vec4& getVec4(std::string name);
    /// @brief Get mat4 of given name
    glm::mat4& getMat4(std::string name);

private:
    static unsigned int idCount;
    unsigned int ID;
    bool enableInstancing;
    RenderType renderType;
    Shader* shader;
    std::string name;

    std::unordered_map<std::string, Texture*> textures;
    std::unordered_map<std::string, Cubemap*> cubemaps;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, glm::vec3> vec3s;
    std::unordered_map<std::string, glm::vec4> vec4s;
    std::unordered_map<std::string, glm::mat4> mat4s;
};

#endif // _MATERIAL_HPP
