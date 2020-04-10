#ifndef _SHADEREXCEPTIONS_HPP
#define _SHADEREXCEPTIONS_HPP

#include <exception>

/**
 * @brief Shader compilation failure exception type class
 * 
 */
class ShaderCompilationFailure : public std::exception
{
private:
    const char* log;
public:
    ShaderCompilationFailure(const char* log) : log(log) {}

    const char* infoLog() const 
    { 
        return log; 
    }
    virtual const char* what() const throw()
    {
        return "Shader compliation failure\n";
    }
};

/**
 * @brief Shader info type error excetption
 * 
 */
class WrongShaderInfoType : public std::exception
{
private:
    const char* msg;
public:
    WrongShaderInfoType(const char* msg) : msg(msg) {}

    virtual const char* what() const throw()
    {
        return msg;
    }
};

#endif // _SHADEREXCEPTIONS_HPP
