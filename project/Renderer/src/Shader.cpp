#include "Shader.hpp"

std::string readTextFile(const char* filePath)
{
    std::string buffer = "";
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(filePath);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        buffer = stream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << "File " << filePath << " could not be read." << '\n';
    }

    return buffer;
}

Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode, const char* geometryShaderCode)
{
    // * Shader compilation ==============================================
    unsigned int vertex, fragment, geometry;
    int success;
    char infoLog[512];

    // TODO: Decide if the exceptions should be caught or left to crash xd
    vertex = compileShader(vertexShaderCode, GL_VERTEX_SHADER);
    fragment = compileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
    if (geometryShaderCode != nullptr)
    {
        // Compile geometry shader only if the code is provided
        geometry = compileShader(geometryShaderCode, GL_GEOMETRY_SHADER);
    }

    // * Shader program creation ==========================================
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryShaderCode != nullptr)
    {
        glAttachShader(ID, geometry);
    }

    glLinkProgram(ID);

    // Handle linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        // TODO: Add message bus debug error log
        
        std::cerr << "Shader program linking error\n" << infoLog << '\n';
    }

    // Get info about shader variables
    retrieveShaderInfo(GL_ACTIVE_ATTRIBUTES, attributes);
    retrieveShaderInfo(GL_ACTIVE_UNIFORMS, uniforms);

    // * Delete the unncecessary shader programs =========================
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryShaderCode != nullptr)
    {
        glDeleteShader(geometry);
    }
}

unsigned int Shader::compileShader(const char* shaderCode, GLenum shaderType)
{
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        // TODO: Change this to support message bus debug logs
        std::cerr << "Error! Shader compilation failed!\n" << infoLog << '\n';

        throw ShaderCompilationFailure(infoLog);
    }

    return shader;
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::retrieveShaderInfo(GLenum shaderInfoType, std::unordered_map<std::string, GLenum> &buffer)
{
    if (shaderInfoType == GL_ACTIVE_ATTRIBUTES || shaderInfoType == GL_ACTIVE_UNIFORMS)
    {
        GLint count;
        glGetProgramiv(ID, shaderInfoType, &count);

        // ! Getting appropriate variable names from shader
        // * Buffer size handled in constexpr
        GLchar name[NAME_BUF_SIZE]; // Variable name in GLSL
        GLsizei length; // Name length
        GLint size; // Size of the variable
        GLenum type; // Type of the variable

        for (GLint i = 0; i < count; i++)
        {
            if (shaderInfoType == GL_ACTIVE_UNIFORMS)
            {
                glGetActiveUniform(ID, i, NAME_BUF_SIZE, &length, &size, &type, name);
                buffer[std::string(name)] = type; // Insert keay value pair
            }
            else if (shaderInfoType == GL_ACTIVE_ATTRIBUTES)
            {
                glGetActiveAttrib(ID, i, NAME_BUF_SIZE, &length, &size, &type, name);
                buffer[std::string(name)] = type;
            }
        }
    }
    else
    {
        throw WrongShaderInfoType("Function received wrong shader info type\n");
    }
}