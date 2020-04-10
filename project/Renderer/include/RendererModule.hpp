#ifndef _RENDERERMODULE_HPP
#define _RENDERERMODULE_HPP

#include "IModule.inl"
#include "Shader.hpp"
#include "Mesh.hpp"

class GLFWWindow;

class RendererModule : IModule
{
public:
    RendererModule();
    ~RendererModule();
    void init(GLFWWindow* window);
    void render();

private:
    GLFWWindow* window;
};

#endif // _RENDERERMODULE_HPP
