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

#include "ShaderExceptions.hpp"

constexpr GLsizei NAME_BUF_SIZE = 32;

/**
 * @brief Shader class, responsible for compiling shaders and creating shader programs
 * 
 */
class Shader
{
public:
    /**
     * @brief Construct a new Shader object
     * 
     * @param vertexShaderCode String with code contents of vertex shader
     * @param fragmentShaderCode String with code contents of fragment shader
     * @param geometryShader String with code contents of geometry shader
     */
    Shader(const char* vertexShaderCode, const char* fragmentShaderCode, const char* geometryShaderCode = nullptr);
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
    
    std::unordered_map<std::string, GLenum> &getAttributes() { return attributes; }
    std::unordered_map<std::string, GLenum> &getUniforms() { return uniforms; }

    void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec3(const std::string& name, const glm::vec3& vec) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setVec4(const std::string& name, const glm::vec4& vec) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    unsigned int ID;
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
