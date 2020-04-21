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

    //TODO documentation
class Material
{
public:
    //TODO documentation
    Material(Shader* shader);
    Material() = default;
    ~Material() = default;

    /**
     * @brief Sets all uniforms and binds appropriate shader program
     */
    void use();

    // Member functions for handling uniforms and textures
    //TODO documentation
    void setTexture(std::string name, Texture value);
    //TODO documentation
    void setInt(std::string name, int value);
    //TODO documentation
    void setFloat(std::string name, float value);
    //TODO documentation
    void setVec3(std::string name, glm::vec3 value);
    //TODO documentation
    void setVec4(std::string name, glm::vec4 value);
    //TODO documentation
    void setMat4(std::string name, glm::mat4 value);

    //TODO documentation
    Shader* getShaderPtr();
    //TODO documentation
    int getInt(std::string name);
    //TODO documentation
    float& getFloat(std::string name);
    //TODO documentation
    glm::vec3& getVec3(std::string name);
    //TODO documentation
    glm::vec4& getVec4(std::string name);
    //TODO documentation
    glm::mat4& getMat4(std::string name);

    int ID;
    Shader* shader;
private:
    static int idCount;


    std::unordered_map<std::string, Texture> textures;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, glm::vec3> vec3s;
    std::unordered_map<std::string, glm::vec4> vec4s;
    std::unordered_map<std::string, glm::mat4> mat4s;
};

#endif // _MATERIAL_HPP
