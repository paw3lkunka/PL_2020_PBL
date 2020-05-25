#include "Shader.hpp"
#include "ShaderExceptions.hpp"

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVec3(const std::string& name, const glm::vec3& vec) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void Shader::setVec4(const std::string& name, const glm::vec4& vec) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name), (int)value);
}
void Shader::setInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}
void Shader::setFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}
void Shader::setVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}
void Shader::setVec3(const char* name, const glm::vec3& vec) const
{
	glUniform3f(glGetUniformLocation(ID, name), vec.x, vec.y, vec.z);
}
void Shader::setVec4(const char* name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}
void Shader::setVec4(const char* name, const glm::vec4& vec) const
{
	glUniform4f(glGetUniformLocation(ID, name), vec.x, vec.y, vec.z, vec.w);
}
void Shader::setMat4(const char* name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode, const char* geometryShaderCode, bool serialize) 
    : ISerializable(serialize)
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

void Shader::retrieveShaderInfo(GLenum shaderInfoType, std::unordered_map<std::string, GLenum>& buffer)
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