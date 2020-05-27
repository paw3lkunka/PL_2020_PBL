#include "SceneReader.hpp"
#include "ObjectModule.hpp"
#include "ObjectExceptions.inl"
#include "FileStructures.inl"
#include "Core.hpp"
#include "Collider.inl"
#include "Camera.inl"

#include <fstream>
#include <iostream>

bool SceneReader::hasInstance = false;

SceneReader::SceneReader(ObjectModule* objModPtr) : objModulePtr(objModPtr) 
{
    if(hasInstance)
    {
        throw TooManyInstancesException("SceneReader");
    }
    hasInstance = true;
}

void SceneReader::readScene(std::string filePath)
{
    std::ifstream file(filePath);
    if(file.good())
    {
        file >> j;
    }
    file.close();
    readMeshes();
    readShaders();
    readTextures();
    readCubemaps();
    readMaterials();
    readEntities();
    readComponents();
}

void SceneReader::readShaders()
{
    int shadersAmount = j.at("Amounts").at("shaders").get<int>();
    std::string name = "";
    std::string fragmentShaderPath;
    std::string vertexShaderPath;
    std::string geometryShaderPath = "";

    Shader* shader;
    
    for(int i = 0; i < shadersAmount; ++i)
    {
        name = setName("shader", i);
        fragmentShaderPath = j.at(name).at("fragmentShaderPath").get<std::string>();
        vertexShaderPath = j.at(name).at("vertexShaderPath").get<std::string>();

        std::cout << fragmentShaderPath << std::endl;
        std::cout << vertexShaderPath << std::endl;
        
        try
        {
            geometryShaderPath = j.at(name).at("geometryShaderPath").get<std::string>();
            shader = objModulePtr->newShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), geometryShaderPath.c_str());
        }
        catch(nlohmann::detail::out_of_range)
        {
            std::cout << "Caught exception" << std::endl;
            shader = objModulePtr->newShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
        }
        shader->serializationID = j.at(name).at("serializationID").get<unsigned int>();
    }
}

void SceneReader::readTextures()
{
    std::string name = "";
    TextureCreateInfo texCreateInfo;
    int texturesAmount = j.at("Amounts").at("textures").get<int>();
    for(int i = 0; i < texturesAmount; ++i)
    {
        name = setName("texture", i);
        texCreateInfo.generateMipmaps = j.at(name).at("creationInfo").at("generateMipmaps").get<bool>();
        texCreateInfo.magFilter = GLenum(j.at(name).at("creationInfo").at("magFilter").get<int>());
        texCreateInfo.minFilter = GLenum(j.at(name).at("creationInfo").at("minFilter").get<int>());
        texCreateInfo.wrapMode = GLenum(j.at(name).at("creationInfo").at("wrapMode").get<int>());

        std::string path = j.at(name).at("filePath").get<std::string>();
        auto tex = objModulePtr->newTexture(path.c_str(), texCreateInfo);
        tex->serializationID = j.at(name).at("serializationID").get<unsigned int>();
    }

}

void SceneReader::readCubemaps()
{
    int cubemapsAmount = j.at("Amounts").at("cubemaps").get<int>();
    std::string name;
    std::string backPath, frontPath, leftPath, rightPath, topPath, bottomPath;
    TextureCreateInfo texCreateInfo;
    for(int i = 0; i < cubemapsAmount; ++i)
    {
        name = setName("cubemap", i);
        frontPath = j.at(name).at("frontPath").get<std::string>();
        bottomPath = j.at(name).at("bottomPath").get<std::string>();
        leftPath = j.at(name).at("leftPath").get<std::string>();
        rightPath = j.at(name).at("rightPath").get<std::string>();
        topPath = j.at(name).at("topPath").get<std::string>();
        backPath = j.at(name).at("backPath").get<std::string>();

        texCreateInfo.generateMipmaps = j.at(name).at("creationInfo").at("generateMipmaps").get<bool>();
        texCreateInfo.format = GLenum(j.at(name).at("creationInfo").at("format").get<unsigned int>());
        texCreateInfo.width = j.at(name).at("creationInfo").at("width").get<int>();
        texCreateInfo.height = j.at(name).at("creationInfo").at("height").get<int>();
        texCreateInfo.magFilter = GLenum(j.at(name).at("creationInfo").at("magFilter").get<unsigned int>());
        texCreateInfo.minFilter = GLenum(j.at(name).at("creationInfo").at("minFilter").get<unsigned int>());
        texCreateInfo.wrapMode = GLenum(j.at(name).at("creationInfo").at("wrapMode").get<unsigned int>());

        auto cubemap = objModulePtr->newCubemap(texCreateInfo, frontPath.c_str(), leftPath.c_str(), rightPath.c_str(), backPath.c_str(), topPath.c_str(), bottomPath.c_str());
        cubemap->serializationID = j.at(name).at("serializationID").get<unsigned int>();
    }
}

void SceneReader::readMaterials()
{
    std::string name;
    int materialsAmount = j.at("Amounts").at("materials").get<int>();
    std::unordered_map<std::string, unsigned int> children;
    std::unordered_map<std::string, int> ints;
    std::unordered_map<std::string, float> floats;
    std::unordered_map<std::string, std::vector<float>> structMap;
    unsigned int shaderID;

    for(int i = 0; i < materialsAmount; ++i)
    {
        name = setName("material", i);
        children.clear();


        shaderID = j.at(name).at("shaderSerializationID").get<unsigned int>();
        auto shader = objModulePtr->objectContainer.getShaderFromSerializationID(shaderID);
        auto instancingEnabled = j.at(name).at("instancingEnabled").get<bool>();
        auto renderingType = j.at(name).at("renderingType").get<int>();
        auto matName = j.at(name).at("name").get<std::string>();

        auto material = objModulePtr->newMaterial(shader, matName, (RenderType)renderingType, instancingEnabled);
        material->serializationID = j.at(name).at("serializationID").get<unsigned int>();


        j.at(name).at("cubemaps").get_to(children);
        for(auto iter : children)
        {
            unsigned int cubemapID = iter.second;
            auto cubemap = objModulePtr->objectContainer.getCubemapFromSerializationID(cubemapID);
            material->setCubemap(iter.first, cubemap);
        }

        children.clear();
        j.at(name).at("textures").get_to(children);
        for(auto iter : children)
        {
            unsigned int texID = iter.second;
            auto texture = objModulePtr->objectContainer.getTextureFromSerializationID(texID);
            material->setTexture(iter.first, texture);
        }

        ints.clear();
        j.at(name).at("ints").get_to(ints);

        for(auto iter : ints)
        {
            material->setInt(iter.first, iter.second);
        }

        floats.clear();
        j.at(name).at("floats").get_to(floats);

        for(auto iter : floats)
        {
            material->setFloat(iter.first, iter.second);
        }

        structMap.clear();
        j.at(name).at("vec3s").get_to(structMap);
        for(auto iter : structMap)
        {
            std::string uniName = iter.first;
            glm::vec3 vector(iter.second[0], iter.second[1], iter.second[2]);
            material->setVec3(uniName, vector);
        }

        structMap.clear();
        j.at(name).at("vec4s").get_to(structMap);
        for(auto iter : structMap)
        {
            std::string uniName = iter.first;
            glm::vec4 vector(iter.second[1], iter.second[2], iter.second[3], iter.second[0]); //!x, y, z, w
            material->setVec4(uniName, vector);
        }

        structMap.clear();
        j.at(name).at("mat4s").get_to(structMap);
        for(auto iter : structMap)
        {
            std::string uniName = iter.first;
            glm::vec4 c0(iter.second[0], iter.second[1], iter.second[2], iter.second[3]);
            glm::vec4 c1(iter.second[4], iter.second[5], iter.second[6], iter.second[7]);
            glm::vec4 c2(iter.second[8], iter.second[9], iter.second[10], iter.second[11]);
            glm::vec4 c3(iter.second[12], iter.second[13], iter.second[14], iter.second[15]);
            
            glm::mat4 matrix(c0, c1, c2, c3);
            material->setMat4(uniName, matrix);
        }
    }
}

void SceneReader::readMeshes()
{
    std::string name;
    int meshesAmount = j.at("Amounts").at("meshes");
    std::string filePath, meshPath;
    for(int i = 0; i < meshesAmount; ++i)
    {
        name = setName("mesh", i);
        filePath = j.at(name).at("filePath").get<std::string>();
        meshPath = j.at(name).at("meshPath").get<std::string>();

        auto meshType = j.at(name).at("type").get<std::string>();
        if(meshType == "MeshCustom")
        {
            objModulePtr->newModel(filePath.c_str());
            auto mesh = objModulePtr->getMeshCustomPtrByPath(meshPath.c_str());
            mesh->serializationID = j.at(name).at("serializationID").get<unsigned int>();
        }
        else if(meshType == "MeshSkinned")
        {
            objModulePtr->newModel(filePath.c_str());
            auto mesh = objModulePtr->getMeshSkinnedPtrByPath(meshPath.c_str());
            mesh->serializationID = j.at(name).at("serializationID").get<unsigned int>();
        }
        else;
    }
}

void SceneReader::readComponents()
{
    int componentsAmount = j.at("Amounts").at("components");
    std::string componentType;
    std::string name;
    for(int i = 0; i < componentsAmount; ++i)
    {
        name = setName("component", i);
        componentType = j.at(name).at("type").get<std::string>();
        if(componentType == "Transform")
        {
            std::cout << "Transform" << std::endl;
            readTransform(name);
        }
        else if(componentType == "AudioSource")
        {
            std::cout << "AudioSource" << std::endl;
            readAudioSource(name);
        }
        else if(componentType == "AudioListener")
        {
            std::cout << "AudioListener" << std::endl;
            readAudioListener(name);
        }
        else if(componentType == "Camera")
        {
            std::cout << "Camera" << std::endl;
            readCamera(name);
        }
        else if(componentType == "MeshRenderer")
        {
            std::cout << "MeshRenderer" << std::endl;
            readMeshRenderer(name);
        }
        else if(componentType == "SphereCollider")
        {
            std::cout << "SphereCollider" << std::endl;
            readSphereCollider(name);
        }
        else if(componentType == "BoxCollider")
        {
            std::cout << "BoxCollider" << std::endl;
            readBoxCollider(name);
        }
        else if(componentType == "Rigidbody")
        {
            std::cout << "Rigidbody" << std::endl;
            readRigidbody(name);
        }
        else if(componentType == "Light")
        {
            std::cout << "Light" << std::endl;
            readLight(name);
        }
        else if(componentType == "PhysicalInputKeymap")
        {
            std::cout << "PhysicalInputKeymap" << std::endl;
            readPhysicalInputKeymap(name);
        }
        else if(componentType == "Paddle")
        {
            std::cout << "Paddle" << std::endl;
            readPaddle(name);
        }
        else if(componentType == "Bone")
        {
            std::cout << "Bone" << std::endl;
            readBone(name);
        }
        else if(componentType == "Skeleton")
        {
            std::cout << "Skeleton" << std::endl;
            readSkeleton(name);
        }
    }

    for(int i = 0; i < componentsAmount; ++i)
    {
        name = setName("component", i);
        componentType = j.at(name).at("type").get<std::string>();
        if(componentType == "Transform")
        {
            readTransformParents(name);
        }
    }
}

void SceneReader::readEntities()
{
    int entitiesAmount = j.at("Amounts").at("entities");
    std::vector<unsigned int> components;
    std::string name;
    for(int i = 0; i < entitiesAmount; ++i)
    {
        name = setName("entity", i);
        j.at(name).at("components").get_to(components);

        auto entityName = j.at(name).at("name").get<std::string>();
        auto entity = objModulePtr->newEntity(components.size(), entityName);
        entity->serializationID = j.at(name).at("serializationID").get<unsigned int>();
        entity->id = j.at(name).at("id").get<unsigned int>();
    }
}

std::string SceneReader::setName(std::string name, int index)
{
    if(index < 10)
    {
        return name += "00" + std::to_string(index);
    }
    else if(index < 100)
    {
        return name += "0" + std::to_string(index);
    }
    else
    {
        return name += std::to_string(index);
    }
    
}

void SceneReader::readTransform(std::string name)
{
    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    auto serializationID = j.at(name).at("serializationID").get<unsigned int>();
    auto component = entity->getComponentPtr<Transform>();
    glm::vec3 tempVec;
    glm::quat tempRot;
    Transform* trans;
    if(component != nullptr)
    {
        trans = component;
    }
    else
    {
        trans = objModulePtr->newEmptyComponent<Transform>();
        assignToEntity(name, trans);
    }
    trans->serializationID = serializationID;


    tempVec.x = j.at(name).at("localPosition").at("x").get<float>();
    tempVec.y = j.at(name).at("localPosition").at("y").get<float>();
    tempVec.z = j.at(name).at("localPosition").at("z").get<float>();
    trans->getLocalPositionModifiable() = tempVec;

    tempVec.x = j.at(name).at("localScale").at("x").get<float>();
    tempVec.y = j.at(name).at("localScale").at("y").get<float>();
    tempVec.z = j.at(name).at("localScale").at("z").get<float>();
    trans->getLocalScaleModifiable() = tempVec;

    
    tempRot.w = j.at(name).at("localRotation").at("w").get<float>();
    tempRot.x = j.at(name).at("localRotation").at("x").get<float>();
    tempRot.y = j.at(name).at("localRotation").at("y").get<float>();
    tempRot.z = j.at(name).at("localRotation").at("z").get<float>();
    trans->getLocalRotationModifiable() = tempRot;
} 

void SceneReader::readAudioSource(std::string name)
{
    auto aSource = objModulePtr->newEmptyComponent<AudioSource>();
    aSource->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    aSource->getConeInnerAngleModifiable() = j.at(name).at("coneInnerAngle").get<float>();
    aSource->getConeOuterAngleModifiable() = j.at(name).at("coneOuterAngle").get<float>();
    aSource->getConeOuterGainModifiable() = j.at(name).at("coneOuterGain").get<float>();
    aSource->getIsLoopingModifiable() = j.at(name).at("isLooping").get<int>();
    aSource->getIsRelativeModifiable() = j.at(name).at("isRelativeToListener").get<int>();
    if(j.at(name).at("maxDistance").get<std::string>() == "infinity")
    {
        aSource->getMaxDistanceModifiable() = std::numeric_limits<float>::infinity();
    }
    else
    {
        aSource->getMaxDistanceModifiable() = j.at(name).at("maxDistance").get<float>();
    }
    aSource->getMaxGainModifiable() = j.at(name).at("maxGain").get<float>();
    aSource->getMinGainModifiable() = j.at(name).at("minGain").get<float>();
    aSource->getGainModifiable() = j.at(name).at("gain").get<float>();
    aSource->getPitchModifiable() = j.at(name).at("pitch").get<float>();
    aSource->getReferenceDistanceModifiable() = j.at(name).at("referenceDistance").get<float>();
    aSource->getRolloffFactorModifiable() = j.at(name).at("rolloffFactor").get<float>();

    glm::vec3 tempVec;
    tempVec.x = j.at(name).at("direction").at("x");
    tempVec.y = j.at(name).at("direction").at("y");
    tempVec.z = j.at(name).at("direction").at("z");

    std::vector<std::string> clips;

    j.at(name).at("clips").get_to(clips);
    for(int i = 0; i < clips.size(); ++i)
    {
        aSource->getClipsModifiable().push_back(clips[i]);
        objModulePtr->newAudioClip(clips[i].c_str());
    }

    std::vector<int> listeners;
    j.at(name).at("listeners").get_to(listeners);

    for(int i = 0; i < listeners.size(); ++i)
    {
        aSource->getListenersModifiable().push_back(dynamic_cast<AudioListener*>(objModulePtr->objectContainer.getComponentFromSerializationID(listeners[i])));
    }

    assignToEntity(name, aSource);
}

void SceneReader::readAudioListener(std::string name)
{
    auto audioListener = objModulePtr->newEmptyComponent<AudioListener>();
    audioListener->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    glm::vec3 tempVec;
    tempVec.x = j.at(name).at("at").at("x");
    tempVec.y = j.at(name).at("at").at("y");
    tempVec.z = j.at(name).at("at").at("z");
    audioListener->getAtModifiable() = tempVec;

    tempVec.x = j.at(name).at("up").at("x");
    tempVec.y = j.at(name).at("up").at("y");
    tempVec.z = j.at(name).at("up").at("z");
    audioListener->getUpModifiable() = tempVec;

    audioListener->getIsCurrentModifiable() = j.at(name).at("isCurrent").get<int>();
    audioListener->getGainModifiable() = j.at(name).at("gain").get<float>();

    assignToEntity(name, audioListener);
}

void SceneReader::readCamera(std::string name)
{
    auto camera = objModulePtr->newEmptyComponent<Camera>();
    camera->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    camera->farPlane = j.at(name).at("farPlane").get<float>();
    camera->fieldOfView = j.at(name).at("fieldOfView").get<float>();
    camera->nearPlane = j.at(name).at("nearPlane").get<float>();
    camera->orthographicSize = j.at(name).at("orthographicSize").get<float>();
    camera->projectionMode = CameraProjection(j.at(name).at("projectionMode").get<int>());
    camera->isMain = j.at(name).at("isMain").get<bool>();

    assignToEntity(name, camera);
}

void SceneReader::readMeshRenderer(std::string name)
{
    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    auto component = entity->getComponentPtr<MeshRenderer>();

    auto serializationID = j.at(name).at("serializationID").get<unsigned int>();
    if(component != nullptr) // * if component exists (if was made by mesh processing)
    {
        auto renderer = dynamic_cast<MeshRenderer*>(component);
        unsigned int childID = j.at(name).at("material").get<unsigned int>();
        renderer->material = objModulePtr->objectContainer.getMaterialFromSerializationID(childID);
        renderer->serializationID = serializationID;
        return;
    }
    else // * if component was made by core
    {
        auto renderer = objModulePtr->newEmptyComponent<MeshRenderer>();
        renderer->serializationID = serializationID;

        unsigned int childID = j.at(name).at("material").get<unsigned int>();
        renderer->material = objModulePtr->objectContainer.getMaterialFromSerializationID(childID);

        childID = j.at(name).at("mesh").get<unsigned int>();
        renderer->mesh = objModulePtr->objectContainer.getMeshFromSerializationID(childID);

        assignToEntity(name, renderer);
    }
}

void SceneReader::readSphereCollider(std::string name)
{
    auto sphereCollider = objModulePtr->newEmptyComponent<SphereCollider>();
    sphereCollider->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    glm::vec3 center;
    center.x = j.at(name).at("center").at("x").get<float>();
    center.y = j.at(name).at("center").at("y").get<float>();
    center.z = j.at(name).at("center").at("z").get<float>();
    
    sphereCollider->center = center;
    sphereCollider->radius = j.at(name).at("radius").get<float>();
    sphereCollider->type = Collider::Type(j.at(name).at("colliderType").get<unsigned int>());

    assignToEntity(name, sphereCollider);
}

void SceneReader::readBoxCollider(std::string name)
{
    auto boxCollider = objModulePtr->newEmptyComponent<BoxCollider>();
    boxCollider->serializationID = j.at(name).at("serializationID").get<unsigned int>();
    
    boxCollider->center.x = j.at(name).at("center").at("x").get<float>();
    boxCollider->center.y = j.at(name).at("center").at("y").get<float>();
    boxCollider->center.z = j.at(name).at("center").at("z").get<float>();
    
    boxCollider->halfSize.x = j.at(name).at("halfSize").at("x").get<float>();
    boxCollider->halfSize.y = j.at(name).at("halfSize").at("y").get<float>();
    boxCollider->halfSize.z = j.at(name).at("halfSize").at("z").get<float>();
    
    boxCollider->type = Collider::Type(j.at(name).at("colliderType").get<unsigned int>());

    boxCollider->calculateVert();

    assignToEntity(name, boxCollider);
}

void SceneReader::readLight(std::string name)
{
    auto light = objModulePtr->newEmptyComponent<Light>();
    light->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    light->lightType = (LightType)j.at(name).at("lightType").get<int>();
    light->intensity = j.at(name).at("intensity").get<float>();
    light->color.r = j.at(name).at("color").at("r").get<float>();
    light->color.g = j.at(name).at("color").at("g").get<float>();
    light->color.b = j.at(name).at("color").at("b").get<float>();
    if (light->lightType == LightType::Point || light->lightType == LightType::Spot)
    {
        light->range = j.at(name).at("range").get<float>();
        if (light->lightType == LightType::Spot)
        {
            light->angle = j.at(name).at("angle").get<float>();
        }
    }

    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    entity->addComponent(light);
}

void SceneReader::readRigidbody(std::string name)
{
    auto rigidbody = objModulePtr->newEmptyComponent<Rigidbody>();
    rigidbody->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    rigidbody->mass = j.at(name).at("mass").get<float>();

    rigidbody->momentOfInertia[0][0] = j.at(name).at("momentOfInertia").at("0,0").get<float>();
    rigidbody->momentOfInertia[1][1] = j.at(name).at("momentOfInertia").at("1,1").get<float>();
    rigidbody->momentOfInertia[2][2] = j.at(name).at("momentOfInertia").at("2,2").get<float>();
    rigidbody->invertedMomentOfInertia = glm::inverse(rigidbody->momentOfInertia);

    rigidbody->drag = j.at(name).at("drag").get<float>();
    rigidbody->angularDrag = j.at(name).at("angularDrag").get<float>();
    rigidbody->ignoreGravity = j.at(name).at("ignoreGravity").get<bool>();

    assignToEntity(name, rigidbody);
}

void SceneReader::readPaddle(std::string name)
{
    auto paddle = objModulePtr->newEmptyComponent<Paddle>();
    paddle->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    paddle->minSpeed = j.at(name).at("minSpeed").get<float>();
    paddle->maxSpeed = j.at(name).at("maxSpeed").get<float>();
    paddle->maxFrontRot = j.at(name).at("maxFrontRot").get<float>();
    paddle->maxSideRot = j.at(name).at("maxSideRot").get<float>();
    paddle->maxPos.x = j.at(name).at("maxPos").at("x").get<float>();
    paddle->maxPos.y = j.at(name).at("maxPos").at("y").get<float>();
    paddle->maxPos.z = j.at(name).at("maxPos").at("z").get<float>();

    assignToEntity(name, paddle);
}

void SceneReader::readBone(std::string name)
{
    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    auto serializationID = j.at(name).at("serializationID").get<unsigned int>();
    auto component = entity->getComponentPtr<Bone>();
    component->serializationID = serializationID;
}

void SceneReader::readSkeleton(std::string name)
{
    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    auto serializationID = j.at(name).at("serializationID").get<unsigned int>();
    auto component = entity->getComponentPtr<Skeleton>();
    component->serializationID = serializationID;
}

void SceneReader::assignToEntity(std::string name, Component* component)
{
    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    entity->addComponent(component);
}

void SceneReader::readPhysicalInputKeymap(std::string name)
{
    auto keymap = objModulePtr->newEmptyComponent<PhysicalInputKeymap>();
    keymap->serializationID = j.at(name).at("serializationID").get<unsigned int>();

    int singleSize = j.at(name).at("single").at("size").get<int>();
    int continuousSize = j.at(name).at("continuous").at("size").get<int>();

    for (int i = 0; i < singleSize; i++)
    {
        int keycode = j.at(name).at("single").at("key " + std::to_string(i)).at("keycode").get<int>();
        keymap->single[keycode].force.x = j.at(name).at("single").at("key " + std::to_string(i)).at("force").at("x").get<float>();
        keymap->single[keycode].force.y = j.at(name).at("single").at("key " + std::to_string(i)).at("force").at("y").get<float>();
        keymap->single[keycode].force.z = j.at(name).at("single").at("key " + std::to_string(i)).at("force").at("z").get<float>();
        keymap->single[keycode].force.x = j.at(name).at("single").at("key " + std::to_string(i)).at("force").at("x").get<float>();
        keymap->single[keycode].force.y = j.at(name).at("single").at("key " + std::to_string(i)).at("force").at("y").get<float>();
        keymap->single[keycode].force.z = j.at(name).at("single").at("key " + std::to_string(i)).at("force").at("z").get<float>();
    }

    for (int i = 0; i < continuousSize; i++)
    {
        int keycode = j.at(name).at("continuous").at("key " + std::to_string(i)).at("keycode").get<int>();
        keymap->continuous[keycode].force.x = j.at(name).at("continuous").at("key " + std::to_string(i)).at("force").at("x").get<float>();
        keymap->continuous[keycode].force.y = j.at(name).at("continuous").at("key " + std::to_string(i)).at("force").at("y").get<float>();
        keymap->continuous[keycode].force.z = j.at(name).at("continuous").at("key " + std::to_string(i)).at("force").at("z").get<float>();
        keymap->continuous[keycode].force.x = j.at(name).at("continuous").at("key " + std::to_string(i)).at("force").at("x").get<float>();
        keymap->continuous[keycode].force.y = j.at(name).at("continuous").at("key " + std::to_string(i)).at("force").at("y").get<float>();
        keymap->continuous[keycode].force.z = j.at(name).at("continuous").at("key " + std::to_string(i)).at("force").at("z").get<float>();
    }

    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    entity->addComponent(keymap);
}

void SceneReader::readTransformParents(std::string name)
{
    unsigned int entityID = j.at(name).at("entity id").get<unsigned int>();
    auto entity = objModulePtr->objectContainer.getEntityFromID(entityID);
    std::cout << "EntityID: " << entityID << '\t';
    auto serializationID = j.at(name).at("serializationID").get<unsigned int>();
    std::cout << "SerializationID: " << serializationID << '\n';
    auto trans = entity->getComponentPtr<Transform>();
    try
    {
        auto parentID = j.at(name).at("transform parentID").get<unsigned int>();
        if(parentID != 0)
        {
            auto parentTrans = dynamic_cast<Transform*>(objModulePtr->objectContainer.getComponentFromSerializationID(parentID));
            trans->setParent(parentTrans);
        }
        else
        {
            trans->setParent(&GetCore().sceneModule.rootNode);
        }
    }
    catch(nlohmann::detail::out_of_range)
    {
        std::cout << "No parent transform for " << name << std::endl;
    }
}
