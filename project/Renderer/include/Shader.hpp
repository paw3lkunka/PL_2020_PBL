/**
 * @file Shader.hpp
 * @author Filip Mazurek (216835@edu.p.lodz.pl)
 * @brief Shader class and helper functions
 * @version 0.1
 * @date 2020-04-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef _SHADER_HPP
#define _SHADER_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <iostream>

#include "ISerializable.hpp"

constexpr GLsizei NAME_BUF_SIZE = 32;

/**
 * @brief Shader class, responsible for compiling shaders and creating shader programs
 */
class Shader : public ISerializable
{
public:
    /**
     * @brief Construct a new Shader object
     * 
     * @param name name of the shader
     * @param vertexShaderCode String with code contents of vertex shader
     * @param fragmentShaderCode String with code contents of fragment shader
     * @param geometryShaderCode String with code contents of geometry shader
     * @param serialize should it be serialized
     */
    Shader(std::string name, const char* vertexShaderCode, const char* fragmentShaderCode, const char* geometryShaderCode = nullptr, bool serialize = true);

    Shader() = default;
    /**
     * @brief Destroy the Shader object
     * 
     */
    ~Shader() = default;

    /**
     * @brief Switch the shader currently used by OpenGL to this
     * 
     */
    void use();
    
    //TODO documentation
    std::unordered_map<std::string, GLenum> &getAttributes() { return attributes; }
    //TODO documentation
    std::unordered_map<std::string, GLenum> &getUniforms() { return uniforms; }

    //TODO documentation
    void setBool(const std::string& name, bool value) const;
    //TODO documentation
	void setInt(const std::string& name, int value) const;
    //TODO documentation
	void setFloat(const std::string& name, float value) const;
    //TODO documentation
	void setVec3(const std::string& name, float x, float y, float z) const;
    //TODO documentation
	void setVec3(const std::string& name, const glm::vec3& vec) const;
    //TODO documentation
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
    //TODO documentation
	void setVec4(const std::string& name, const glm::vec4& vec) const;
    //TODO documentation
	void setMat4(const std::string& name, const glm::mat4& mat) const;
    //TODO documentation
	void setMat3(const std::string& name, const glm::mat3& mat) const;

    //TODO documentation
    void setBool(const char* name, bool value) const;
    //TODO documentation
	void setInt(const char* name, int value) const;
    //TODO documentation
	void setFloat(const char* name, float value) const;
    //TODO documentation
	void setVec3(const char* name, float x, float y, float z) const;
    //TODO documentation
	void setVec3(const char* name, const glm::vec3& vec) const;
    //TODO documentation
	void setVec4(const char* name, float x, float y, float z, float w) const;
    //TODO documentation
	void setVec4(const char* name, const glm::vec4& vec) const;
    //TODO documentation
	void setMat4(const char* name, const glm::mat4& mat) const;
    //TODO documentation
	void setMat3(const char* name, const glm::mat3& mat) const;

    unsigned int ID;

    std::string vertexShaderPath = "";
    std::string fragmentShaderPath = "";
    std::string geometryShaderPath = "";
    const std::string shaderName;

private:
    std::unordered_map<std::string, GLenum> attributes;
    std::unordered_map<std::string, GLenum> uniforms;


    /**
     * @brief Retrieves info about shader variables - uniforms and attributes
     * 
     * @param shaderInfoType Variable type to get, should be GL_ACTIVE_ATTRIBUTES or GL_ACTIVE_UNIFORMS
     * @param buffer A buffer to write to
     */
    void retrieveShaderInfo(GLenum shaderInfoType, std::unordered_map<std::string, GLenum> &buffer);
    /**
     * @brief Compiles single shader from code provided
     * 
     * @param shaderCode Self explanatory shader code
     * @param shaderType Type of shader to be compiled - currently only vertex, fragment and geometry supported
     * @return unsigned int The ID of a shader to be included into a shader program
     */
    unsigned int compileShader(const char* shaderCode, GLenum shaderType);
};

#endif // _SHADER_HPP
