#include "ObjectContainer.hpp"
#include "ObjectModule.hpp"
#include "ObjectMaker.hpp"
#include "ObjectExceptions.inl"

#include "Entity.hpp"
#include "Component.hpp"
#include "Shader.hpp"
#include "Cubemap.hpp"
#include "CubemapHdr.hpp"
#include "Material.hpp"
#include "Transform.hh"
#include "Font.hpp"
#include "Mesh.hpp"
#include "MeshCustom.hpp"
#include "MeshSkinned.hpp"

bool ObjectContainer::hasInstance = false;

ObjectContainer::ObjectContainer(ObjectModule* objModule) : objModule(objModule) 
{
    if(hasInstance)
    {
        throw TooManyInstancesException("ObjectContainer");
    }
    hasInstance = true;
    shaders.reserve(50);
    components.reserve(1500);
    entities.reserve(600);
    meshes.reserve(150);
    textures.reserve(200);
    cubemaps.reserve(10);
    hdrCubemaps.reserve(10);
    materials.reserve(50);
    fonts.reserve(16);
}

void ObjectContainer::cleanup()
{
    for(auto c : components)
    {
        delete c;
        c = nullptr;
    }
    components.clear();
    
    for(auto m : meshes)
    {
        delete m;
        m = nullptr;
    }
    meshes.clear();

    entities.clear();

    for(auto s : shaders)
    {
        delete s;
        s = nullptr;
    }
    shaders.clear();

    for(auto t : textures)
    {
        delete t;
        t = nullptr;
    }
    textures.clear();

    for(auto m : materials)
    {
        delete m;
        m = nullptr;
    }
    materials.clear();

    for(auto c : cubemaps)
    {
        delete c;
        c = nullptr;
    }
    cubemaps.clear();

    for(auto c : hdrCubemaps)
    {
        delete c;
        c = nullptr;
    }
    hdrCubemaps.clear();

    for(auto f : fonts)
    {
        delete f;
        f = nullptr;
    }
    fonts.clear();
}

MeshCustom* ObjectContainer::getMeshCustomFromPath(const char* meshPath)
{
    for(auto m : meshes)
    {
        if((m->getMeshPath().compare(meshPath) == 0) && dynamic_cast<MeshCustom*>(m) != nullptr)
        {
            return dynamic_cast<MeshCustom*>(m);
        }
    }
    throw FileNotFoundException(meshPath);
}

MeshSkinned* ObjectContainer::getMeshSkinnedFromPath(const char* meshPath)
{
    for(auto m : meshes)
    {
        if((m->getMeshPath().compare(meshPath) == 0) && dynamic_cast<MeshSkinned*>(m) != nullptr)
        {
            return dynamic_cast<MeshSkinned*>(m);
        }
    }
    throw FileNotFoundException(meshPath);
}

Entity* ObjectContainer::getEntityFromID(unsigned int entityID)
{
    for(int i = 0; i < entities.size(); ++i)
    {
        if(entities[i].getId() == entityID)
        {
            return &entities[i];
        }
    }
    return nullptr;
}

Entity* ObjectContainer::getEntityFromName(const char* name)
{
    for(int i = 0; i < entities.size(); ++i)
    {
        if(entities[i].getName().compare(name) == 0 )
        {
            return &entities[i];
        }
    }
    return nullptr;
}

Entity* ObjectContainer::getEntityFromSerializationID(unsigned int serializationID)
{
    for(int i = 0; i < entities.size(); ++i)
    {
        if(entities[i].serializationID == serializationID)
        {
            return &entities[i];
        }
    }
    return nullptr;
}

Component* ObjectContainer::getComponentFromSerializationID(unsigned int serializationID)
{
    Component* sejakas = nullptr;
    for(auto c : components)
    {
        //std::cout << "Looking for ID " << serializationID << ", checking ID " << c->serializationID << '\n'; 
        if(c->serializationID == serializationID)
        {
            //std::cout << "SERIALIZATION ID " << serializationID << "FOUND!\n";
            sejakas = c;
            //std::cout << "GetComponentFromSerializationID: " << serializationID << " sejakas is null: " << (sejakas == nullptr) << '\n';
            break;
        }
    }
    return sejakas;
}

Shader* ObjectContainer::getShaderFromSerializationID(unsigned int serializationID)
{
    for(auto s : shaders)
    {
        if(s->serializationID == serializationID)
        {
            return s;
        }
    }
    return nullptr;
}

Mesh* ObjectContainer::getMeshFromSerializationID(unsigned int serializationID)
{
    for(auto m : meshes)
    {
        if(m->serializationID == serializationID)
        {
            return m;
        }
    }
    return nullptr;
}

Texture* ObjectContainer::getTextureFromSerializationID(unsigned int serializationID)
{
    for(auto t : textures)
    {
        if(t->serializationID == serializationID)
        {
            return t;
        }
    }
    return nullptr;
}

Cubemap* ObjectContainer::getCubemapFromSerializationID(unsigned int serializationID)
{
    for(auto c : cubemaps)
    {
        if(c->serializationID == serializationID)
        {
            return c;
        }
    }
    return nullptr;
}

CubemapHdr* ObjectContainer::getCubemapHdrFromSerializationID(unsigned int serializationID)
{
    for(auto c : hdrCubemaps)
    {
        if(c->serializationID == serializationID)
        {
            return c;
        }
    }
    return nullptr;
}

Material* ObjectContainer::getMaterialFromSerializationID(unsigned int serializationID)
{
    for(auto m : materials)
    {
        if(m->serializationID == serializationID)
        {
            return m;
        }
    }
    return nullptr;
}

Material* ObjectContainer::getMaterialFromName(const char* name)
{
    for(auto m : materials)
    {
        if(strcmp(m->getName(), name) == 0)
        {
            return m;
        }
    }
    return nullptr;
}

Texture* ObjectContainer::getTexturePtrByFilePath(const char* filePath)
{
    for(Texture* t : textures)
    {
        if(t->filePath.compare(filePath) == 0)
        {
            return t;
        }
    }
    return nullptr;
}

Font* ObjectContainer::getFontFromSerializationID(unsigned int serializationID)
{
    for(auto f : fonts)
    {
        if(f->serializationID == serializationID)
        {
            return f;
        }
    }
    return nullptr;
}

Animation* ObjectContainer::getAnimationPtrByName(const char* name)
{
    return &animations.at(name);
}

Font* ObjectContainer::getFontPtrByName(const char* name)
{
    Font* font = nullptr;
    for(auto var : fonts)
    {
        if (strcmp(var->getName(), name) == 0)
        {
            font = var;
            break;
        }
    }
    return font;
}

Shader* ObjectContainer::getShaderPtrByName(std::string shaderName)
{
    Shader* shader = nullptr;
    for(auto var : shaders)
    {
        if (var->shaderName.compare(shaderName) == 0)
        {
            shader = var;
            break;
        }
    }
    return shader;
}

Mesh* ObjectContainer::getMeshByMeshPath(std::string meshPath)
{
    Mesh* mesh = nullptr;
    for(auto var : meshes)
    {
        if(meshPath.compare(var->getMeshPath()) == 0)
        {
            mesh = var;
            break;
        }
    }
    return mesh;
}

void ObjectContainer::unloadScene()
{
    for(int i = components.size() - 1; i > 0; --i)
    {
        delete components[i];
        components[i] = nullptr;
    }
    components.clear();
    entities.clear();
}