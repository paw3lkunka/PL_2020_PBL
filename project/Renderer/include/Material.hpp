/**
 * @file Material.hpp
 * @author Filip Mazurek (216835@edu.p.lodz.pl)
 * @brief Material class for shader settings abstraction
 * @version 0.1
 * @date 2020-04-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

#include "MeshDataStructures.inl"
#include "Texture.hpp"
#include "Shader.hpp"

class Material
{
public:
    Material(Shader* shader);
    Material() = default;
    ~Material() = default;

    /**
     * @brief Sets all uniforms and binds appropriate shader program
     */
    void use();

    // Member functions for handling uniforms and textures
    void setTexture(std::string name, Texture value);
    void setInt(std::string name, int value);
    void setFloat(std::string name, float value);
    void setVec3(std::string name, glm::vec3 value);
    void setVec4(std::string name, glm::vec4 value);
    void setMat4(std::string name, glm::mat4 value);

    Shader* getShaderPtr();
    int getInt(std::string name);
    float& getFloat(std::string name);
    glm::vec3& getVec3(std::string name);
    glm::vec4& getVec4(std::string name);
    glm::mat4& getMat4(std::string name);

private:
    static int idCount;

    int ID;
    Shader* shader;

    std::unordered_map<std::string, Texture> textures;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, glm::vec3> vec3s;
    std::unordered_map<std::string, glm::vec4> vec4s;
    std::unordered_map<std::string, glm::mat4> mat4s;
};

#endif // _MATERIAL_HPP
