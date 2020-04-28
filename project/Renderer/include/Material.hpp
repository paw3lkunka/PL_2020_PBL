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
#include "ISerializable.inl"

#include "Shader.hpp"

/**
 * @brief Class encapsulating shader program and uniform variables
 */
class Material: public ISerializable
{
public:
    /**
     * @brief Construct a new Material object
     * 
     * @param shader Shader to draw with
     */
    Material(Shader* shader);
    Material() = default;
    ~Material() = default;

    /**
     * @brief Sets all uniforms and binds appropriate shader program
     */
    void use();

    /// @brief Set texture of given name in material
    void setTexture(std::string name, Texture value);
    /// @brief Set texture of given name in material
    void setCubemap(std::string name, Cubemap value);
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
    static int idCount;
    int ID;
    Shader* shader;

    std::unordered_map<std::string, Texture> textures;
    std::unordered_map<std::string, Cubemap> cubemaps;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, glm::vec3> vec3s;
    std::unordered_map<std::string, glm::vec4> vec4s;
    std::unordered_map<std::string, glm::mat4> mat4s;
};

#endif // _MATERIAL_HPP
