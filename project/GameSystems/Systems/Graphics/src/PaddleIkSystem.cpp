#include "PaddleIkSystem.hpp"
#include "Components.inc"
#include "Entity.hpp"
#include "Core.hpp"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/epsilon.hpp>

bool PaddleIkSystem::assertEntity(Entity* entity)
{
    endBonePtr = entity->getComponentPtr<Transform>();
    bonePointPtr = entity->getComponentPtr<IKBonePoint>();
    bonePtr = entity->getComponentPtr<Bone>();
    
    return (bonePtr != nullptr && bonePointPtr != nullptr && endBonePtr != nullptr);
}

void PaddleIkSystem::start()
{
    init();
}

void PaddleIkSystem::frameUpdate()
{
    resolveIK();
}

void PaddleIkSystem::init()
{
    std::cout << "Entity: " << Name(bonePtr) << std::endl;
    int arraysLength = bonePointPtr->chainLength;
    IKBone** bones = new IKBone*[arraysLength];
    float completeLength = 0;

    Transform* current = endBonePtr;
    //* initialize data; bones[0] is chain root bone, last is the nearest bone to target
    for(int i = arraysLength - 1; i >= 0; --i)
    {
        bones[i] = new IKBone();
        bones[i]->boneTrans = const_cast<Transform*>(current->getParent());
        bones[i]->tipBoneTrans = current;
        std::cout << "BoneTrans: " << bones[i]->boneTrans->entityPtr->getName() << std::endl;
        std::cout << "TipBoneTrans: " << bones[i]->tipBoneTrans->entityPtr->getName() << std::endl;
        bones[i]->startRot = rotationInWorld( bones[i]->boneTrans );
        bones[i]->endPos = positionInWorld( current );
        bones[i]->startPos = positionInWorld( bones[i]->boneTrans );
        bones[i]->length = glm::length(bones[i]->endPos - bones[i]->startPos);
        completeLength += bones[i]->length;

        bones[i]->dir = glm::normalize(bones[i]->endPos - bones[i]->startPos);

        // ? Bone rendering
        Entity* e = GetCore().objectModule.newEntity(2, Name(bonePtr) + std::to_string(i));
        MeshRenderer* meshRenderer = GetCore().objectModule.newEmptyComponent<MeshRenderer>();
            meshRenderer->mesh = GetCore().objectModule.getMeshCustomPtrByPath("Resources/Models/BoneBox.FBX/Box001");
            Shader* sh = GetCore().objectModule.getMaterialPtrByName("paddleMat")->getShaderPtr();
            Material* mat = GetCore().objectModule.newMaterial(sh, "bones", RenderType::Opaque);
            mat->setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            meshRenderer->material = mat;
        e->addComponent(meshRenderer);
        Transform* t = GetCore().objectModule.newEmptyComponentForLastEntity<Transform>();
        t->setParent(const_cast<Transform*>(current->getParent()));
        t->getLocalRotationModifiable() = eulerToQuaternion({0, 90, 0});
        t->getLocalScaleModifiable() = {1.0f, 1.0f, bones[i]->length};
        
        current = const_cast<Transform*>(current->getParent());
    }
    //? root bone position - attached to rest of the body
    glm::vec3 rootPos = positionInWorld(current);
    chainMappings[bonePtr->entityPtr->serializationID] = IKChain(bones, arraysLength, completeLength, rootPos);
}

void PaddleIkSystem::resolveIK()
{
    std::cout << "Entity: " << Name(bonePtr) << std::endl;
    IKChain chain = chainMappings.at(bonePtr->entityPtr->serializationID);
    Transform* target = bonePointPtr->touchPoint;
    glm::vec3 targetPos = positionInWorld(target);
    //* Set position and rotation
    for(int i = 0; i < chain.chainLength; ++i)
    {
        chain.bones[i]->endPos = positionInWorld( chain.bones[i]->tipBoneTrans);
        chain.bones[i]->startPos = positionInWorld( chain.bones[i]->boneTrans );
        chain.bones[i]->rotation = rotationInWorld( chain.bones[i]->boneTrans );
    }

    union 
    {
        glm::vec3 shit3;
        glm::vec4 shit4;
    }sh;

    for(int i = 0; i < chain.chainLength; ++i)
    {
        IKBone* cBone = chain.bones[i];
        cBone->endPos = glm::mix(cBone->startPos, cBone->endPos + cBone->dir, bonePointPtr->snapBackStrength);
        if(i < chain.chainLength - 1)
        {
            chain.bones[i + 1]->startPos = cBone->endPos;
        }
    }
    
    for(int iter = 0; iter < bonePointPtr->iterations; ++iter)
    {
        //* Set rotations and positions to target - from tip to root
        for(int i = chain.chainLength - 1; i >= 0; --i)
        {
            IKBone* cBone = chain.bones[i];
            //? last bone - pointing at target
            if(i == chain.chainLength - 1) 
            {
                cBone->endPos = targetPos;
                //? count direction - position has changed so it's new directional vector
                glm::vec3 direction = glm::normalize(cBone->endPos - cBone->startPos);
                cBone->startPos = cBone->endPos + (direction * cBone->length);
            }
            else 
            {
                //? set tip position to last bone start position
                cBone->endPos = chain.bones[i + 1]->startPos;
                //? count direction - position has changed so it's new directional vector
                glm::vec3 direction = glm::normalize(cBone->endPos - cBone->startPos);
                cBone->startPos = cBone->endPos + (direction * cBone->length);
            }
        }

        //* Set rotations and positions to root - from root to tip
        for(int i = 0; i < chain.chainLength; ++i)
        {
            IKBone* cBone = chain.bones[i];
            //? first bone - attach to root
            if(i == 0) 
            {
                glm::vec3 direction = glm::normalize(cBone->startPos - cBone->endPos);
                cBone->startPos = chain.rootPos;
                cBone->endPos = cBone->startPos + direction * cBone->length;
            }
            else
            {
                glm::vec3 direction = glm::normalize(cBone->startPos - cBone->endPos);
                cBone->startPos = chain.bones[i - 1]->endPos;
                cBone->endPos = cBone->startPos + direction * cBone->length;
            }
            
        }
        if(glm::length2(chain.bones[chain.chainLength - 1]->endPos - targetPos) < bonePointPtr->delta * bonePointPtr->delta)
        {
            break;
        }
    }

    //* set positions and rotations to bones
    for(int i = 0; i < chain.chainLength; ++i)
    {
        IKBone* cBone = chain.bones[i];
        cBone->boneTrans->getLocalPositionModifiable() = positionInLocal(cBone->boneTrans, cBone->startPos);
        auto direction = glm::normalize(cBone->startPos - cBone->endPos);
        cBone->rotation = cBone->startRot * glm::quatLookAt(direction, {0, 1, 0});
        cBone->boneTrans->getLocalRotationModifiable() = rotationInLocal(cBone->boneTrans, cBone->rotation);
        GetCore().sceneModule.process(*cBone->boneTrans, true);
    }
    IKBone* tipBone = chain.bones[chain.chainLength - 1];
    tipBone->tipBoneTrans->getLocalPositionModifiable() = positionInLocal(tipBone->tipBoneTrans, tipBone->endPos);
}

glm::vec3 PaddleIkSystem::positionInWorld(Transform* trans)
{
    return trans->getParentMatrix() * glm::vec4(trans->getLocalPosition(), 1.0f);
}

glm::vec3 PaddleIkSystem::positionInLocal(Transform* trans, glm::vec3 pos)
{
    return trans->getToParentMatrix() * glm::vec4(pos, 1.0f);
}

glm::quat PaddleIkSystem::rotationInWorld(Transform* current)
{
    return current->getWorldRotation();
}

glm::quat PaddleIkSystem::rotationInLocal(Transform* current, glm::quat rot)
{
    return rot * glm::inverse(current->getParent()->getWorldRotation());
}

glm::quat PaddleIkSystem::rotateVectors(glm::vec3 from, glm::vec3 to)
{
    from = glm::normalize(from);
    to = glm::normalize(to);
    float cosAngle = glm::dot(from, to);
    glm::clamp(cosAngle, -1.0f, 1.0f);
    float angle = glm::acos(cosAngle);
    glm::vec3 rotAxis = glm::cross(from, to);
    return glm::toQuat(glm::rotate(glm::mat4(1.0f), angle, rotAxis));
}
