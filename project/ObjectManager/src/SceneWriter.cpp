#include "SceneWriter.hpp"
#include "Components.inc"
#include "ObjectModule.hpp"
#include "ObjectContainer.hpp"
#include "ObjectExceptions.inl"

#include "Material.hpp"
#include "Texture.hpp"
#include "Cubemap.hpp"
#include "CubemapHdr.hpp"
#include "MeshCustom.hpp"
#include "MeshSkinned.hpp"
#include "Shader.hpp"
#include "Entity.hpp"
#include "Font.hpp"
#include "Message.inl"

#include <fstream>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>

bool SceneWriter::hasInstance = false;

SceneWriter::SceneWriter(ObjectModule* objectModulePtr)
{
    if(hasInstance)
    {
        throw TooManyInstancesException("SceneWriter");
    }
    hasInstance = true;
    objModulePtr = objectModulePtr;
    objContainerPtr = &objectModulePtr->objectContainer;
}

void SceneWriter::saveScene(const char* filePath)
{
    json = new nlohmann::json();
    (*json)["Amounts"]["entities"] = objContainerPtr->entities.size();
    (*json)["Amounts"]["shaders"] = objContainerPtr->shaders.size();
    (*json)["Amounts"]["materials"] = objContainerPtr->materials.size();
    (*json)["Amounts"]["components"] = objContainerPtr->components.size();
    (*json)["Amounts"]["meshes"] = objContainerPtr->meshes.size();
    (*json)["Amounts"]["textures"] = objContainerPtr->textures.size();
    (*json)["Amounts"]["cubemaps"] = objContainerPtr->cubemaps.size();
    (*json)["Amounts"]["cubemapsHdr"] = objContainerPtr->hdrCubemaps.size();
    (*json)["Amounts"]["fonts"] = objContainerPtr->fonts.size();
    
    for( int i = 0; i < objContainerPtr->entities.size(); ++i)
    {
        childrenID.clear();
        if(i < 10)
        {
            name = "entity00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "entity0" + std::to_string(i);
        }
        else
        {
            name = "entity" + std::to_string(i);
        }
        (*json)[name]["id"] = objContainerPtr->entities[i].getId();
        (*json)[name]["name"] = objContainerPtr->entities[i].getName();
        (*json)[name]["serializationID"] = objContainerPtr->entities[i].serializationID;
        for(int j = 0; j < objContainerPtr->entities[i].getComponentsPtr()->size(); ++j)
        {
            childrenID.push_back(objContainerPtr->entities[i].getComponentsPtr()->operator[](j)->serializationID);
        }
        (*json)[name]["components"] = childrenID;
    }

    for(int i = 0; i < objContainerPtr->shaders.size(); ++i)
    {
        if(i < 10)
        {
            name = "shader00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "shader0" + std::to_string(i);
        }
        else
        {
            name = "shader" + std::to_string(i);
        }
        saveShader(objContainerPtr->shaders[i]);
    }

    for(int i = 0; i < objContainerPtr->materials.size(); ++i)
    {
        if(i < 10)
        {
            name = "material00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "material0" + std::to_string(i);
        }
        else
        {
            name = "material" + std::to_string(i);
        }
        saveMaterial(objContainerPtr->materials[i]);
    }

    for(int i = 0; i < objContainerPtr->meshes.size(); ++i)
    {
        if(i < 10)
        {
            name = "mesh00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "mesh0" + std::to_string(i);
        }
        else
        {
            name = "mesh" + std::to_string(i);
        }
        saveMesh(objContainerPtr->meshes[i]);
    }

    for(int i = 0; i < objContainerPtr->textures.size(); ++i)
    {
        if(i < 10)
        {
            name = "texture00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "texture0" + std::to_string(i);
        }
        else
        {
            name = "texture" + std::to_string(i);
        }
        saveTexture(objContainerPtr->textures[i]);
    }

    for(int i = 0; i < objContainerPtr->cubemaps.size(); ++i)
    {
        if(i < 10)
        {
            name = "cubemap00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "cubemap0" + std::to_string(i);
        }
        else
        {
            name = "cubemap" + std::to_string(i);
        }
        saveCubemap(objContainerPtr->cubemaps[i]);
    }

    for(int i = 0; i < objContainerPtr->hdrCubemaps.size(); ++i)
    {
        if(i < 10)
        {
            name = "cubemapHdr00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "cubemapHdr0" + std::to_string(i);
        }
        else
        {
            name = "cubemapHdr" + std::to_string(i);
        }
        saveCubemapHdr(objContainerPtr->hdrCubemaps[i]);
    }

    for(int i = 0; i < objContainerPtr->fonts.size(); ++i)
    {
        if(i < 10)
        {
            name = "font00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "font0" + std::to_string(i);
        }
        else
        {
            name = "font" + std::to_string(i);
        }
        saveFont(objContainerPtr->fonts[i]);
    }

    for( int i = 0; i < objContainerPtr->components.size(); ++i)
    {
        if(i < 10)
        {
            name = "component00" + std::to_string(i);
        }
        else if(i < 100)
        {
            name = "component0" + std::to_string(i);
        }
        else
        {
            name = "component" + std::to_string(i);
        }

        (*json)[name]["entity id"] = objContainerPtr->components[i]->entityPtr->getId();
        (*json)[name]["serializationID"] = objContainerPtr->components[i]->serializationID;
        if(Transform* temp = dynamic_cast<Transform*>(objContainerPtr->components[i]))
        {
            saveTransform(temp);
        }
        else if(AudioSource* temp = dynamic_cast<AudioSource*>(objContainerPtr->components[i]))
        {
            saveAudioSource(temp);
        }
        else if(AudioListener* temp = dynamic_cast<AudioListener*>(objContainerPtr->components[i]))
        {
            saveAudioListener(temp);
        }
        else if(Camera* temp = dynamic_cast<Camera*>(objContainerPtr->components[i]))
        {
            saveCamera(temp);
        }
        else if(ThirdPersonCamera* temp = dynamic_cast<ThirdPersonCamera*>(objContainerPtr->components[i]))
        {
            saveThirdPersonCamera(temp);
        }
        else if(FirstPersonCamera* temp = dynamic_cast<FirstPersonCamera*>(objContainerPtr->components[i]))
        {
            saveFirstPersonCamera(temp);
        }
        else if(MeshRenderer* temp = dynamic_cast<MeshRenderer*>(objContainerPtr->components[i]))
        {
            saveMeshRenderer(temp);
        }
        else if(TerrainRenderer* temp = dynamic_cast<TerrainRenderer*>(objContainerPtr->components[i]))
        {
            saveTerrainRenderer(temp);
        }
        else if(SphereCollider* temp = dynamic_cast<SphereCollider*>(objContainerPtr->components[i]))
        {
            saveSphereCollider(temp);
        }
        else if(BoxCollider* temp = dynamic_cast<BoxCollider*>(objContainerPtr->components[i]))
        {
            saveBoxCollider(temp);
        }
        else if(Rigidbody* temp = dynamic_cast<Rigidbody*>(objContainerPtr->components[i]))
        {
            saveRigidbody(temp);
        }
        else if(Light* temp = dynamic_cast<Light*>(objContainerPtr->components[i]))
        {
            saveLight(temp);
        }
        else if(PhysicalInputKeymap* temp = dynamic_cast<PhysicalInputKeymap*>(objContainerPtr->components[i]))
        {
            savePhysicalInputKeymap(temp);
        }
        else if(Paddle* temp = dynamic_cast<Paddle*>(objContainerPtr->components[i]))
        {
            savePaddle(temp);
        }
        else if(Enemy* temp = dynamic_cast<Enemy*>(objContainerPtr->components[i]))
        {
            saveEnemy(temp);
        }
        else if(EnemyAnimation* temp = dynamic_cast<EnemyAnimation*>(objContainerPtr->components[i]))
        {
            saveEnemyAnimation(temp);
        }
        else if(EnemyAttack* temp = dynamic_cast<EnemyAttack*>(objContainerPtr->components[i]))
        {
            saveEnemyAttack(temp);
        }
        else if(UiRenderer* temp = dynamic_cast<UiRenderer*>(objContainerPtr->components[i]))
        {
            saveUiRenderer(temp);
        }
        else if(TextRenderer* temp = dynamic_cast<TextRenderer*>(objContainerPtr->components[i]))
        {
            saveTextRenderer(temp);
        }
        else if(Button* temp = dynamic_cast<Button*>(objContainerPtr->components[i]))
        {
            saveButton(temp);
        }
        else if(RectTransform* temp = dynamic_cast<RectTransform*>(objContainerPtr->components[i]))
        {
            saveRectTransform(temp);
        }
        else if(UiSortingGroup* temp = dynamic_cast<UiSortingGroup*>(objContainerPtr->components[i]))
        {
            saveUiSortingGroup(temp);
        }
        else if(CargoButton* temp = dynamic_cast<CargoButton*>(objContainerPtr->components[i]))
        {
            saveCargoButton(temp);
        }
        else if(ToggleButton* temp = dynamic_cast<ToggleButton*>(objContainerPtr->components[i]))
        {
            saveToggleButton(temp);
        }
        else if(CargoStorage* temp = dynamic_cast<CargoStorage*>(objContainerPtr->components[i]))
        {
            saveCargoStorage(temp);
        }
        else if(dynamic_cast<Cargo*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "Cargo";
        }
        else if(dynamic_cast<Skeleton*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "Skeleton";
        }
        else if(dynamic_cast<Bone*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "Bone";
        }
        else if(dynamic_cast<HydroBody*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "HydroBody";
        }
        else if(HydroAccelerator* temp = dynamic_cast<HydroAccelerator*>(objContainerPtr->components[i]))
        {
            saveHydroAccelerator(temp);
        }
        else if(dynamic_cast<HydroCurrent*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "HydroCurrent";
        }
        else if(dynamic_cast<Kayak*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "Kayak";
        }
        else if(dynamic_cast<Hideout*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "Hideout";
        }
        else if(dynamic_cast<ProgressBar*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "ProgressBar";
        }
        else if(dynamic_cast<Shoot*>(objContainerPtr->components[i]))
        {
            (*json)[name]["type"] = "Shoot";
        }
    }

    std::ofstream file(filePath);
    if(file.good())
    {
        file << std::setw(4) << *json;
    }
    file.close();

    delete json;
    json = nullptr;
}

#pragma region Components
void SceneWriter::saveTransform(Transform* componentPtr)
{
    (*json)[name]["type"] = "Transform";
    if(componentPtr->getParent() != nullptr)
        (*json)[name]["transform parentID"] = componentPtr->getParent()->serializationID;

    (*json)[name]["localPosition"]["x"] = componentPtr->getLocalPosition().x;
    (*json)[name]["localPosition"]["y"] = componentPtr->getLocalPosition().y;
    (*json)[name]["localPosition"]["z"] = componentPtr->getLocalPosition().z;

    (*json)[name]["localRotation"]["x"] = componentPtr->getLocalRotation().x;
    (*json)[name]["localRotation"]["y"] = componentPtr->getLocalRotation().y;
    (*json)[name]["localRotation"]["z"] = componentPtr->getLocalRotation().z;
    (*json)[name]["localRotation"]["w"] = componentPtr->getLocalRotation().w;

    (*json)[name]["localScale"]["x"] = componentPtr->getLocalScale().x;
    (*json)[name]["localScale"]["y"] = componentPtr->getLocalScale().y;
    (*json)[name]["localScale"]["z"] = componentPtr->getLocalScale().z;
}

void SceneWriter::saveAudioListener(AudioListener* componentPtr)
{
    (*json)[name]["type"] = "AudioListener";
    (*json)[name]["gain"] = componentPtr->getGain();
    (*json)[name]["at"]["x"] = componentPtr->getAt().x;
    (*json)[name]["at"]["y"] = componentPtr->getAt().y;
    (*json)[name]["at"]["z"] = componentPtr->getAt().z;
    (*json)[name]["up"]["x"] = componentPtr->getUp().x;
    (*json)[name]["up"]["y"] = componentPtr->getUp().y;
    (*json)[name]["up"]["z"] = componentPtr->getUp().z;
}

void SceneWriter::saveAudioSource(AudioSource* componentPtr)
{
    (*json)[name]["type"] = "AudioSource";
    (*json)[name]["audioClip"] = componentPtr->audioClip;
    (*json)[name]["isRelativeToListener"] = componentPtr->getIsRelative();
    (*json)[name]["isLooping"] = componentPtr->getIsLooping();
    (*json)[name]["minGain"] = componentPtr->getMinGain();
    (*json)[name]["gain"] = componentPtr->getGain();
    (*json)[name]["maxGain"] = componentPtr->getMaxGain();
    (*json)[name]["referenceDistance"] = componentPtr->getReferenceDistance();
    (*json)[name]["rolloffFactor"] = componentPtr->getRolloffFactor();
    (*json)[name]["autoPlay"] = componentPtr->autoPlay;
    if(componentPtr->getMaxDistance() != std::numeric_limits<float>::infinity())
    {
        (*json)[name]["maxDistance"] = componentPtr->getMaxDistance();
    }
    else
    {
        (*json)[name]["maxDistance"] = "infinity";
    }
    (*json)[name]["pitch"] = componentPtr->getPitch();
    (*json)[name]["direction"]["x"] = componentPtr->getDirection().x;
    (*json)[name]["direction"]["y"] = componentPtr->getDirection().y;
    (*json)[name]["direction"]["z"] = componentPtr->getDirection().z;
    (*json)[name]["coneInnerAngle"] = componentPtr->getConeInnerAngle();
    (*json)[name]["coneOuterAngle"] = componentPtr->getConeOuterAngle();
    (*json)[name]["coneOuterGain"] = componentPtr->getConeOuterGain();
}

void SceneWriter::saveCamera(Camera* componentPtr)
{
    (*json)[name]["type"] = "Camera";
    (*json)[name]["projectionMode"] = componentPtr->getProjectionMode();
    (*json)[name]["isMain"] = componentPtr->isMain;
    (*json)[name]["camControl"] = componentPtr->control;
    (*json)[name]["fieldOfView"] = componentPtr->getFrustum().fieldOfView;
    (*json)[name]["nearPlane"] = componentPtr->getFrustum().nearPlane;
    (*json)[name]["farPlane"] = componentPtr->getFrustum().farPlane;
}


void SceneWriter::saveThirdPersonCamera(ThirdPersonCamera* componentPtr)
{
    (*json)[name]["type"] = "ThirdPersonCamera";
    
    (*json)[name]["player"] = componentPtr->player->serializationID;
    (*json)[name]["playerOffset"]["x"] = componentPtr->playerOffset.x;
    (*json)[name]["playerOffset"]["y"] = componentPtr->playerOffset.y;
    (*json)[name]["playerOffset"]["z"] = componentPtr->playerOffset.z;

    (*json)[name]["distance"] = componentPtr->distance;
    (*json)[name]["minDistance"] = componentPtr->minDistance;
    (*json)[name]["maxDistance"] = componentPtr->maxDistance;

    (*json)[name]["minPitch"] = componentPtr->minPitch;
    (*json)[name]["maxPitch"] = componentPtr->maxPitch;

    (*json)[name]["moveLerp"] = componentPtr->moveLerp;
    
    (*json)[name]["zoomSensitivity"] = componentPtr->zoomSensitivity;
    (*json)[name]["zoomLerp"] = componentPtr->zoomLerp;

    (*json)[name]["rotationSensitivity"] = componentPtr->rotationSensitivity;
    (*json)[name]["rotationLerp"] = componentPtr->rotationLerp;
}

void SceneWriter::saveFirstPersonCamera(FirstPersonCamera* componentPtr)
{
    (*json)[name]["type"] = "FirstPersonCamera";

    (*json)[name]["player"] = componentPtr->player->serializationID;
    (*json)[name]["headOffset"]["x"] = componentPtr->headOffset.x;
    (*json)[name]["headOffset"]["y"] = componentPtr->headOffset.y;
    (*json)[name]["headOffset"]["z"] = componentPtr->headOffset.z;

    (*json)[name]["rotationSensitivity"] = componentPtr->rotationSensitivity;

    (*json)[name]["minPitch"] = componentPtr->minPitch;
    (*json)[name]["maxPitch"] = componentPtr->maxPitch;

    (*json)[name]["minYaw"] = componentPtr->minYaw;
    (*json)[name]["maxYaw"] = componentPtr->maxYaw;

    (*json)[name]["moveLerp"] = componentPtr->moveLerp;
    (*json)[name]["rotationLerp"] = componentPtr->rotationLerp;
}

void SceneWriter::saveMeshRenderer(MeshRenderer* componentPtr)
{
    (*json)[name]["type"] = "MeshRenderer";
    if(componentPtr->material != nullptr)
    {
        (*json)[name]["material"] = componentPtr->material->getName();
    }
    (*json)[name]["mesh"] = componentPtr->mesh->getMeshPath();
    
}

void SceneWriter::saveTerrainRenderer(TerrainRenderer* componentPtr)
{
    (*json)[name]["type"] = "TerrainRenderer";
    if(componentPtr->material != nullptr)
    {
        (*json)[name]["material"] = componentPtr->material->name;
    }
    (*json)[name]["mesh"] = componentPtr->terrainMesh->getMeshPath();
    (*json)[name]["splatmap"] = componentPtr->splatmap->filePath;
}

void SceneWriter::saveSphereCollider(SphereCollider* componentPtr)
{
    (*json)[name]["type"] = "SphereCollider";
    (*json)[name]["isTrigger"] = componentPtr->isTrigger;
    (*json)[name]["radius"] = componentPtr->radius;
    (*json)[name]["center"]["x"] = componentPtr->center.x;
    (*json)[name]["center"]["y"] = componentPtr->center.y;
    (*json)[name]["center"]["z"] = componentPtr->center.z;
}

void SceneWriter::saveBoxCollider(BoxCollider* componentPtr)
{
    (*json)[name]["type"] = "BoxCollider";
    (*json)[name]["isTrigger"] = componentPtr->isTrigger;
    (*json)[name]["center"]["x"] = componentPtr->center.x;
    (*json)[name]["center"]["y"] = componentPtr->center.y;
    (*json)[name]["center"]["z"] = componentPtr->center.z;
    (*json)[name]["halfSize"]["x"] = componentPtr->halfSize.x;
    (*json)[name]["halfSize"]["y"] = componentPtr->halfSize.y;
    (*json)[name]["halfSize"]["z"] = componentPtr->halfSize.z;
}

void SceneWriter::saveLight(Light* componentPtr)
{
    (*json)[name]["type"] = "Light";
    (*json)[name]["lightType"] = componentPtr->lightType;
    (*json)[name]["intensity"] = componentPtr->intensity;
    (*json)[name]["color"]["r"] = componentPtr->color.r;
    (*json)[name]["color"]["g"] = componentPtr->color.g;
    (*json)[name]["color"]["b"] = componentPtr->color.b;
    if (componentPtr->lightType == LightType::Point || componentPtr->lightType == LightType::Spot)
    {
        (*json)[name]["range"] = componentPtr->range;
        if (componentPtr->lightType == LightType::Spot)
        {
            (*json)[name]["angle"] = componentPtr->angle;
        }
    }
}

void SceneWriter::saveRigidbody(Rigidbody* componentPtr)
{
    (*json)[name]["type"] = "Rigidbody";
    (*json)[name]["mass"] = componentPtr->mass;
    (*json)[name]["bodyType"] = componentPtr->type;;
    (*json)[name]["drag"] = componentPtr->drag;
    (*json)[name]["angularDrag"] = componentPtr->angularDrag;
    (*json)[name]["ignoreGravity"] = componentPtr->ignoreGravity;
}

void SceneWriter::savePhysicalInputKeymap(PhysicalInputKeymap* keymapPtr)
{
    (*json)[name]["type"] = "PhysicalInputKeymap";

    (*json)[name]["single"]["size"] = keymapPtr->single.size();
    (*json)[name]["continuous"]["size"] = keymapPtr->continuous.size();

    int i = 0;

    for (auto& entry : keymapPtr->single)
    {
        (*json)[name]["single"]["key " + std::to_string(i)]["keycode"] = entry.first;

        (*json)[name]["single"]["key " + std::to_string(i)]["force"]["x"] = entry.second.force.x;
        (*json)[name]["single"]["key " + std::to_string(i)]["force"]["y"] = entry.second.force.y;
        (*json)[name]["single"]["key " + std::to_string(i)]["force"]["z"] = entry.second.force.z;

        (*json)[name]["single"]["key " + std::to_string(i)]["point"]["x"] = entry.second.point.x;
        (*json)[name]["single"]["key " + std::to_string(i)]["point"]["y"] = entry.second.point.y;
        (*json)[name]["single"]["key " + std::to_string(i)]["point"]["z"] = entry.second.point.z;
        i++;
    }

    i = 0;

    for (auto& entry : keymapPtr->continuous)
    {
        (*json)[name]["continuous"]["key " + std::to_string(i)]["keycode"] = entry.first;

        (*json)[name]["continuous"]["key " + std::to_string(i)]["force"]["x"] = entry.second.force.x;
        (*json)[name]["continuous"]["key " + std::to_string(i)]["force"]["y"] = entry.second.force.y;
        (*json)[name]["continuous"]["key " + std::to_string(i)]["force"]["z"] = entry.second.force.z;

        (*json)[name]["continuous"]["key " + std::to_string(i)]["point"]["x"] = entry.second.point.x;
        (*json)[name]["continuous"]["key " + std::to_string(i)]["point"]["y"] = entry.second.point.y;
        (*json)[name]["continuous"]["key " + std::to_string(i)]["point"]["z"] = entry.second.point.z;
        i++;
    }
}

void SceneWriter::savePaddle(Paddle* componentPtr)
{
    (*json)[name]["type"] = "Paddle";
    (*json)[name]["minSpeed"] = componentPtr->minSpeed;
    (*json)[name]["maxSpeed"] = componentPtr->maxSpeed;
    (*json)[name]["maxFrontRot"] = componentPtr->maxFrontRot;
    (*json)[name]["maxSideRot"] = componentPtr->maxSideRot;
    (*json)[name]["maxPos"]["x"] = componentPtr->maxPos.x;
    (*json)[name]["maxPos"]["y"] = componentPtr->maxPos.y;
    (*json)[name]["maxPos"]["z"] = componentPtr->maxPos.z;
}

void SceneWriter::saveEnemy(Enemy* enemyPtr)
{
    (*json)[name]["type"] = "Enemy";
    (*json)[name]["sightDistance"] = enemyPtr->sightDistance;
    (*json)[name]["sightAngle"] = enemyPtr->sightAngle;
    (*json)[name]["detectionCounterMaxValue"] = enemyPtr->detectionCounterMaxValue;
    (*json)[name]["detectionPositiveStep"] = enemyPtr->detectionPositiveStep;
    (*json)[name]["detectionNegativeStep"] = enemyPtr->detectionNegativeStep;
    (*json)[name]["detectionCounter"] = enemyPtr->detectionCounter;
}

void SceneWriter::saveEnemyAnimation(EnemyAnimation* enemyAnimPtr)
{
    (*json)[name]["type"] = "EnemyAnimation";
    (*json)[name]["lerpParameter"] = enemyAnimPtr->lerpParameter;
}

void SceneWriter::saveUiRenderer(UiRenderer* componentPtr)
{
    (*json)[name]["type"] = "UiRenderer";
    (*json)[name]["material"] = componentPtr->material->getName();
}

void SceneWriter::saveTextRenderer(TextRenderer* componentPtr)
{
    (*json)[name]["type"] = "TextRenderer";
    (*json)[name]["material"] = componentPtr->material->getName();
    (*json)[name]["font"] = componentPtr->mesh.font->getName();
    (*json)[name]["text"] = componentPtr->mesh.text;
}

void SceneWriter::saveRectTransform(RectTransform* componentPtr)
{
    (*json)[name]["type"] = "RectTransform";

    (*json)[name]["anchor"]["x"] = componentPtr->getAnchor().x;
    (*json)[name]["anchor"]["y"] = componentPtr->getAnchor().y;

    (*json)[name]["localPosition"]["x"] = componentPtr->getLocalPosition().x;
    (*json)[name]["localPosition"]["y"] = componentPtr->getLocalPosition().y;

    (*json)[name]["rectSize"]["x"] = componentPtr->getSize().x;
    (*json)[name]["rectSize"]["y"] = componentPtr->getSize().y;

    (*json)[name]["rotation"] = componentPtr->getLocalRotation();

    if(componentPtr->getParent() != nullptr)
    {
        (*json)[name]["parent"] = componentPtr->getParent()->serializationID;
    }
}

void SceneWriter::saveButton(Button* componentPtr)
{
    (*json)[name]["type"] = "Button";
    (*json)[name]["isActive"] = componentPtr->isActive;
    (*json)[name]["baseColor"]["r"] = componentPtr->baseColor.r;
    (*json)[name]["baseColor"]["g"] = componentPtr->baseColor.g;
    (*json)[name]["baseColor"]["b"] = componentPtr->baseColor.b;
    (*json)[name]["baseColor"]["a"] = componentPtr->baseColor.a;

    (*json)[name]["inactiveColor"]["r"] = componentPtr->inactiveColor.r;
    (*json)[name]["inactiveColor"]["g"] = componentPtr->inactiveColor.g;
    (*json)[name]["inactiveColor"]["b"] = componentPtr->inactiveColor.b;
    (*json)[name]["inactiveColor"]["a"] = componentPtr->inactiveColor.a;

    (*json)[name]["highlightedColor"]["r"] = componentPtr->highlightedColor.r;
    (*json)[name]["highlightedColor"]["g"] = componentPtr->highlightedColor.g;
    (*json)[name]["highlightedColor"]["b"] = componentPtr->highlightedColor.b;
    (*json)[name]["highlightedColor"]["a"] = componentPtr->highlightedColor.a;

    (*json)[name]["onClickColor"]["r"] = componentPtr->onClickColor.r;
    (*json)[name]["onClickColor"]["g"] = componentPtr->onClickColor.g;
    (*json)[name]["onClickColor"]["b"] = componentPtr->onClickColor.b;
    (*json)[name]["onClickColor"]["a"] = componentPtr->onClickColor.a;

    for(int i = 0; i < componentPtr->onClickEvents.size(); ++i)
    {
        std::string eventName = "e" + std::to_string(i);

        saveMessage(eventName, componentPtr->onClickEvents[i]);
    }
}

void SceneWriter::saveHydroAccelerator(HydroAccelerator* componentPtr)
{
    (*json)[name]["type"] = "HydroAccelerator";
    (*json)[name]["rigidbody"] = componentPtr->rigidbody->serializationID;
}

void SceneWriter::saveUiSortingGroup(UiSortingGroup* componentPtr)
{
    (*json)[name]["type"] = "UiSortingGroup";
    (*json)[name]["groupTransparency"] = componentPtr->groupTransparency;
}

void SceneWriter::saveCargo(Cargo* componentPtr, std::string cargoName, nlohmann::json* parser)
{
    (*parser)[cargoName]["name"] = componentPtr->name;
    (*parser)[cargoName]["isRisky"] = componentPtr->isRisky;
    (*parser)[cargoName]["weight"] = componentPtr->weight;
    (*parser)[cargoName]["income"] = componentPtr->income;
}

void SceneWriter::saveToggleButton(ToggleButton* componentPtr)
{
    (*json)[name]["type"] = "ToggleButton";
    (*json)[name]["isActive"] = componentPtr->isActive;

    (*json)[name]["inactiveColor"]["r"] = componentPtr->inactiveColor.r;
    (*json)[name]["inactiveColor"]["g"] = componentPtr->inactiveColor.g;
    (*json)[name]["inactiveColor"]["b"] = componentPtr->inactiveColor.b;
    (*json)[name]["inactiveColor"]["a"] = componentPtr->inactiveColor.a;

    (*json)[name]["baseColorOn"]["r"] = componentPtr->baseColorOn.r;
    (*json)[name]["baseColorOn"]["g"] = componentPtr->baseColorOn.g;
    (*json)[name]["baseColorOn"]["b"] = componentPtr->baseColorOn.b;
    (*json)[name]["baseColorOn"]["a"] = componentPtr->baseColorOn.a;

    (*json)[name]["highlightedColorOn"]["r"] = componentPtr->highlightedColorOn.r;
    (*json)[name]["highlightedColorOn"]["g"] = componentPtr->highlightedColorOn.g;
    (*json)[name]["highlightedColorOn"]["b"] = componentPtr->highlightedColorOn.b;
    (*json)[name]["highlightedColorOn"]["a"] = componentPtr->highlightedColorOn.a;

    (*json)[name]["onClickColorOn"]["r"] = componentPtr->onClickColorOn.r;
    (*json)[name]["onClickColorOn"]["g"] = componentPtr->onClickColorOn.g;
    (*json)[name]["onClickColorOn"]["b"] = componentPtr->onClickColorOn.b;
    (*json)[name]["onClickColorOn"]["a"] = componentPtr->onClickColorOn.a;

    (*json)[name]["baseColorOff"]["r"] = componentPtr->baseColorOff.r;
    (*json)[name]["baseColorOff"]["g"] = componentPtr->baseColorOff.g;
    (*json)[name]["baseColorOff"]["b"] = componentPtr->baseColorOff.b;
    (*json)[name]["baseColorOff"]["a"] = componentPtr->baseColorOff.a;

    (*json)[name]["highlightedColorOff"]["r"] = componentPtr->highlightedColorOff.r;
    (*json)[name]["highlightedColorOff"]["g"] = componentPtr->highlightedColorOff.g;
    (*json)[name]["highlightedColorOff"]["b"] = componentPtr->highlightedColorOff.b;
    (*json)[name]["highlightedColorOff"]["a"] = componentPtr->highlightedColorOff.a;

    (*json)[name]["onClickColorOff"]["r"] = componentPtr->onClickColorOff.r;
    (*json)[name]["onClickColorOff"]["g"] = componentPtr->onClickColorOff.g;
    (*json)[name]["onClickColorOff"]["b"] = componentPtr->onClickColorOff.b;
    (*json)[name]["onClickColorOff"]["a"] = componentPtr->onClickColorOff.a;

    for(int i = 0; i < componentPtr->onActivateEvents.size(); ++i)
    {
        std::string eventName = "e" + std::to_string(i);

        saveMessage(eventName, componentPtr->onActivateEvents[i], "onActivateEvents");
    }

    for(int i = 0; i < componentPtr->onDeactivateEvents.size(); ++i)
    {
        std::string eventName = "e" + std::to_string(i);

        saveMessage(eventName, componentPtr->onDeactivateEvents[i], "onDeactivateEvents");
    }
}

void SceneWriter::saveCargoButton(CargoButton* componentPtr)
{
    saveToggleButton(componentPtr);
    (*json)[name]["type"] = "CargoButton";
    (*json)[name]["nameText"] = componentPtr->nameText->serializationID;
    (*json)[name]["incomeText"] = componentPtr->incomeText->serializationID;
    (*json)[name]["weightText"] = componentPtr->weightText->serializationID;
}

void SceneWriter::saveCargoStorage(CargoStorage* componentPtr)
{
    (*json)[name]["type"] = "CargoStorage";
}

void SceneWriter::saveEnemyAttack(EnemyAttack* componentPtr)
{
    (*json)[name]["type"] = "EnemyAttack";
    (*json)[name]["attackCounter"] = componentPtr->attackCounter;
    (*json)[name]["incrementValue"] = componentPtr->incrementValue;
    (*json)[name]["activationValue"] = componentPtr->activationValue;
    (*json)[name]["successChance"] = componentPtr->successChance;
}

#pragma endregion

#pragma region Assets

void SceneWriter::saveMaterial(Material* assetPtr)
{
    (*json)[name]["serializationID"] = assetPtr->serializationID;
    (*json)[name]["isSkybox"] = assetPtr->isSkybox;
    (*json)[name]["shaderName"] = assetPtr->shader->shaderName;
    (*json)[name]["name"] = assetPtr->getName();
    (*json)[name]["instancingEnabled"] = assetPtr->isInstancingEnabled();
    (*json)[name]["renderingType"] = assetPtr->getRenderType();

    std::unordered_map<std::string, std::string> texMap;
    std::unordered_map<std::string, unsigned int> cubemaps;
    std::unordered_map<std::string, unsigned int> cubemapsHdr;
    for(auto t : assetPtr->textures)
    {
        if (dynamic_cast<Cubemap*>(t.second) != nullptr)
        {
            cubemaps[t.first] = t.second->serializationID;
        }
        else if (dynamic_cast<CubemapHdr*>(t.second) != nullptr)
        {
            cubemapsHdr[t.first] = t.second->serializationID;
        }
        else if (t.second != nullptr)
        {
            texMap[t.first] = t.second->filePath;
        }
    }
    (*json)[name]["textures"] = texMap;
    (*json)[name]["cubemaps"] = cubemaps;
    (*json)[name]["cubemapsHdr"] = cubemapsHdr;

    childrenMap.clear();
    for(auto i : assetPtr->ints)
    {
        childrenMap[i.first] = i.second;
    }
    (*json)[name]["ints"] = childrenMap;

    std::unordered_map<std::string, float> floatMap;

    for(auto f : assetPtr->floats)
    {
        floatMap[f.first] = f.second;
    }
    (*json)[name]["floats"] = floatMap;


    std::unordered_map<std::string, std::vector<float>> structMap;
    std::vector<float> floatVec;
    for(auto v : assetPtr->vec2s)
    {
        floatVec.clear();
        floatVec.push_back(v.second.x);
        floatVec.push_back(v.second.y);
        
        structMap[v.first] = floatVec;
    }
    (*json)[name]["vec2s"] = structMap;

    
    structMap.clear();
    for(auto v : assetPtr->vec3s)
    {
        floatVec.clear();
        floatVec.push_back(v.second.x);
        floatVec.push_back(v.second.y);
        floatVec.push_back(v.second.z);
        
        structMap[v.first] = floatVec;
    }
    (*json)[name]["vec3s"] = structMap;

    structMap.clear();
    for(auto v : assetPtr->vec4s)
    {
        floatVec.clear();
        floatVec.push_back(v.second.w);
        floatVec.push_back(v.second.x);
        floatVec.push_back(v.second.y);
        floatVec.push_back(v.second.z);

        structMap[v.first] = floatVec;
    }
    (*json)[name]["vec4s"] = structMap;

    structMap.clear();
    for(auto m : assetPtr->mat4s)
    {
        floatVec.clear();
        floatVec.push_back(m.second[0][0]);
        floatVec.push_back(m.second[0][1]);
        floatVec.push_back(m.second[0][2]);
        floatVec.push_back(m.second[0][3]);
        floatVec.push_back(m.second[1][0]);
        floatVec.push_back(m.second[1][1]);
        floatVec.push_back(m.second[1][2]);
        floatVec.push_back(m.second[1][3]);
        floatVec.push_back(m.second[2][0]);
        floatVec.push_back(m.second[2][1]);
        floatVec.push_back(m.second[2][2]);
        floatVec.push_back(m.second[2][3]);
        floatVec.push_back(m.second[3][0]);
        floatVec.push_back(m.second[3][1]);
        floatVec.push_back(m.second[3][2]);
        floatVec.push_back(m.second[3][3]);

        structMap[m.first] = floatVec;
    }
    (*json)[name]["mat4s"] = structMap;
}

void SceneWriter::saveTexture(Texture* assetPtr)
{
    (*json)[name]["serializationID"] = assetPtr->serializationID;
    (*json)[name]["filePath"] = assetPtr->filePath;
    (*json)[name]["creationInfo"]["generateMipmaps"] = assetPtr->info.generateMipmaps;
    (*json)[name]["creationInfo"]["minFilter"] = assetPtr->info.minFilter;
    (*json)[name]["creationInfo"]["magFilter"] = assetPtr->info.magFilter;
    (*json)[name]["creationInfo"]["wrapMode"] = assetPtr->info.wrapMode;
}

void SceneWriter::saveMesh(Mesh* assetPtr)
{
    (*json)[name]["serializationID"] = assetPtr->serializationID;
    (*json)[name]["customName"] = assetPtr->customName;
    (*json)[name]["meshPath"] = assetPtr->getMeshPath();
    (*json)[name]["filePath"] = assetPtr->filePath;
    if(dynamic_cast<MeshCustom*>(assetPtr) != nullptr)
    {
        (*json)[name]["type"] = "MeshCustom";
    }
    else if(dynamic_cast<MeshSkinned*>(assetPtr) != nullptr)
    {
        (*json)[name]["type"] = "MeshSkinned";
    }
}

void SceneWriter::saveShader(Shader* assetPtr)
{
    (*json)[name]["shaderName"] = assetPtr->shaderName;
    (*json)[name]["serializationID"] = assetPtr->serializationID;
    (*json)[name]["fragmentShaderPath"] = assetPtr->fragmentShaderPath;
    (*json)[name]["vertexShaderPath"] = assetPtr->vertexShaderPath;
    if(assetPtr->geometryShaderPath != "")
    {
        (*json)[name]["geometryShaderPath"] = assetPtr->geometryShaderPath;
    }
}

void SceneWriter::saveCubemap(Cubemap* assetPtr)
{
    (*json)[name]["serializationID"] = assetPtr->serializationID;
    (*json)[name]["frontPath"] = assetPtr->frontPath;
    (*json)[name]["backPath"] = assetPtr->backPath;
    (*json)[name]["leftPath"] = assetPtr->leftPath;
    (*json)[name]["rightPath"] = assetPtr->rightPath;
    (*json)[name]["topPath"] = assetPtr->topPath;
    (*json)[name]["bottomPath"] = assetPtr->bottomPath;
    (*json)[name]["creationInfo"]["generateMipmaps"] = assetPtr->info.generateMipmaps;
    (*json)[name]["creationInfo"]["format"] = assetPtr->info.format;
    (*json)[name]["creationInfo"]["width"] = assetPtr->info.width;
    (*json)[name]["creationInfo"]["height"] = assetPtr->info.height;
    (*json)[name]["creationInfo"]["minFilter"] = assetPtr->info.minFilter;
    (*json)[name]["creationInfo"]["magFilter"] = assetPtr->info.magFilter;
    (*json)[name]["creationInfo"]["wrapMode"] = assetPtr->info.wrapMode;
}

void SceneWriter::saveCubemapHdr(CubemapHdr* assetPtr)
{
    (*json)[name]["serializationID"] = assetPtr->serializationID;
    (*json)[name]["frontPath"] = assetPtr->frontPath;
    (*json)[name]["backPath"] = assetPtr->backPath;
    (*json)[name]["leftPath"] = assetPtr->leftPath;
    (*json)[name]["rightPath"] = assetPtr->rightPath;
    (*json)[name]["topPath"] = assetPtr->topPath;
    (*json)[name]["bottomPath"] = assetPtr->bottomPath;
    (*json)[name]["creationInfo"]["generateMipmaps"] = assetPtr->info.generateMipmaps;
    (*json)[name]["creationInfo"]["format"] = assetPtr->info.format;
    (*json)[name]["creationInfo"]["width"] = assetPtr->info.width;
    (*json)[name]["creationInfo"]["height"] = assetPtr->info.height;
    (*json)[name]["creationInfo"]["minFilter"] = assetPtr->info.minFilter;
    (*json)[name]["creationInfo"]["magFilter"] = assetPtr->info.magFilter;
    (*json)[name]["creationInfo"]["wrapMode"] = assetPtr->info.wrapMode;
}

void SceneWriter::saveFont(Font* assetPtr)
{
    (*json)[name]["serializationID"] = assetPtr->serializationID;
    (*json)[name]["fontPath"] = assetPtr->getFontPath();
    (*json)[name]["fontSize"] = assetPtr->getSize();
    (*json)[name]["fontName"] = assetPtr->getName();
}

#pragma endregion

#pragma region Events

void SceneWriter::saveMessage(std::string msgName, Message msg, std::string typeName)
{
    int event = -1;
    switch(msg.getEvent())
    {
        case Event::AUDIO_SOURCE_PLAY:
            event = (int)Event::AUDIO_SOURCE_PLAY;
            (*json)[name][typeName.c_str()][msgName.c_str()]["audioSource"] = msg.getValue<AudioSource*>()->serializationID;
        break;
        case Event::AUDIO_SOURCE_STOP:
            event = (int)Event::AUDIO_SOURCE_STOP;
            (*json)[name][typeName.c_str()][msgName.c_str()]["audioSource"] = msg.getValue<AudioSource*>()->serializationID;
        break;
        case Event::AUDIO_SOURCE_PLAY_ONE_SHOT:
            event = (int)Event::AUDIO_SOURCE_PLAY_ONE_SHOT;
            (*json)[name][typeName.c_str()][msgName.c_str()]["audioSource"] = msg.getValue<AudioSource*>()->serializationID;
        break;
        case Event::LOAD_SCENE:
            event = (int)Event::LOAD_SCENE;
            (*json)[name][typeName.c_str()][msgName.c_str()]["scene"] = msg.getValue<const char*>();
        break;
        case Event::EXIT_GAME:
            event = (int)Event::EXIT_GAME;
        break;
    }

    if(event != -1)
    {
        (*json)[name][typeName.c_str()][msgName.c_str()]["event"] = msg.getEvent();
    }
}
#pragma endregion

void SceneWriter::saveChosenCargos(CargoStorage* componentPtr)
{
    if(componentPtr->entityPtr->getComponentPtr<Transform>() == nullptr)
    {
        nlohmann::json* temp = new nlohmann::json();
        int i = 0;
        for(auto cargo : componentPtr->cargosStored)
        {
            std::string cargoName = "c" + std::to_string(i++);
            saveCargo(cargo, cargoName, temp);
        }


        std::ofstream file("Resources/chosenCargos.json");
        if(file.good())
        {
            file << std::setw(4) << *temp;
        }
        file.close();

        delete temp;
    }
}