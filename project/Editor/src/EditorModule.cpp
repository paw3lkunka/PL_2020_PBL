#include "EditorModule.hpp"

#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include <glm/gtx/string_cast.hpp>

#include "MomentOfInertia.hpp"

#include <sstream>
#include <iomanip>

#include "Components.inc"
#include "Entity.hpp"
#include "Core.hpp"


void EditorModule::init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // ? Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    // ? Setup Dear ImGui style
    ImGui::StyleColorsDark();

    objectModule = &GetCore().objectModule;
}

void EditorModule::setup()
{
     // * pointer for entity
    entityPtr = objectModule->getEntityPtrByID(0u);

    sortEntities();
    // * index for combo list
    currentItem = 0;
    enumValue = 0;
}

void EditorModule::drawEditor()
{
    // ? ++++ IMGUI NEW FRAME ++++
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ? +++++ IMGUI WINDOW ++++
    ImGui::Begin("Edit window");

    ImGui::BeginChild(" ", ImVec2(250, 30));
        if(ImGui::Combo("", &currentItem, entities.c_str()))
        {
            entityPtr = objectModule->getEntityPtrByID(getEntityIdFromCombo(currentItem));
        }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Sort by", ImVec2(250, 30));
        ImGui::Text("Sort by: ");
        ImGui::SameLine();
        if(ImGui::Combo("", &enumValue, sortingTypesList))
        {
            sortEntities(SortingType(enumValue));
            currentItem = 0;
            entityPtr = objectModule->getEntityPtrByID(getEntityIdFromCombo(currentItem));
        }
    ImGui::EndChild();

    ImGui::Text(("Entity: " + std::string(entityPtr->getName())).c_str());
    if(Transform* temp = entityPtr->getComponentPtr<Transform>())
    {
        if(ImGui::CollapsingHeader("Transform"))
        {
            drawTransform(temp);
        }
    }
    
    if(Paddle* temp = entityPtr->getComponentPtr<Paddle>())
    {
        if(ImGui::CollapsingHeader("Paddle"))
        {
            drawPaddle(temp);
        }
    }

    if(Light* temp = entityPtr->getComponentPtr<Light>())
    {
        if(ImGui::CollapsingHeader("Light"))
        {
            drawLight(temp);
        }
    }

    if(Bone* temp = entityPtr->getComponentPtr<Bone>())
    {
        if(ImGui::CollapsingHeader("Bone"))
        {
            drawBone(temp);
        }
    }
    
    if(Rigidbody* temp = entityPtr->getComponentPtr<Rigidbody>())
    {
        if(ImGui::CollapsingHeader("Rigidbody"))
        {
            drawRigidbody(temp);
        }
    }

    if(RectTransform* temp = entityPtr->getComponentPtr<RectTransform>())
    {
        if(ImGui::CollapsingHeader("RectTransform"))
        {
            drawRectTransform(temp);
        }
    }

    ImGui::NewLine();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void EditorModule::onExit()
{
    //! IMGUI CLEANUP
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

std::string EditorModule::formatVec3(glm::vec3 vector)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "( " << vector.x << ", " << vector.y << ", " <<vector.z << " )";
    return ss.str();
}

void EditorModule::drawTransform(Transform* transformPtr)
{
    //* transform variables
    glm::vec3 localRotation(0.0f);

    localRotation = glm::eulerAngles(transformPtr->getLocalRotation()) * 180.0f / glm::pi<float>();

    ImGui::Text("Local transform:");
    ImGui::DragFloat3("Position: ", (float*)&transformPtr->getLocalPositionModifiable(), 0.5f, -1000.0f, 1000.0f, "%.2f");
    if(ImGui::DragFloat3("Rotation: ", (float*)&localRotation, 0.5f, -360.0f, 360.0f, "%.1f"))
    {
        transformPtr->getLocalRotationModifiable() = eulerToQuaternion(localRotation);
    }
    ImGui::DragFloat3("Scale: ", (float*)&transformPtr->getLocalScaleModifiable(), 1.0f, 1.0f, 100.0f, "%.2f");
    ImGui::NewLine();
    ImGui::Text("World Transform: ");
    {
        glm::quat worldRot = transformPtr->getWorldRotation();
        
        glm::vec3 worldPos = transformPtr->getParentMatrix() * glm::vec4(transformPtr->getLocalPosition(), 1.0f);
        glm::vec3 worldRotation = glm::eulerAngles(worldRot) * 180.0f / glm::pi<float>();
        glm::vec3 worldScale = transformPtr->getParentMatrix() * glm::vec4(transformPtr->getLocalScale(), 1.0f);
        
        ImGui::Text( ("Position: " + formatVec3(worldPos)).c_str() );
        ImGui::SameLine();
        ImGui::Text( ("Rotation: " + formatVec3(worldRotation)).c_str() );
        ImGui::SameLine();
        ImGui::Text( ("Scale: " + formatVec3(worldScale)).c_str() );
    }
    ImGui::NewLine();
    if(transformPtr->getParent()->serializationID == 0)
    {
        ImGui::Text("Parent name: Root scene");
    }
    else
    {
        ImGui::Text(("Parent name: " + std::string(transformPtr->getParent()->entityPtr->getName())).c_str());
    }
}

void EditorModule::drawRectTransform(RectTransform* rectTransformPtr)
{
    //* transform variables
    static float rotation = 0;
    ImGui::Text("Local transform:");
    ImGui::DragFloat2("Position: ", (float*)&rectTransformPtr->getLocalPositionModifiable(), 1.0f, -100.0f, 2000.0f, "%.2f");
    //ImGui::DragFloat2("Origin: ", (float*)&rectTransformPtr->getOriginModifiable(), 1.0f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat2("Anchor: ", (float*)&rectTransformPtr->getAnchorModifiable(), 1.0f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Rotation: ", &rotation, 1.0f, 0.0f, 360.0f, "%.1f");
    ImGui::DragFloat2("Size: ", (float*)&rectTransformPtr->getSizeModifiable(), 1.0f, 0.0f, 2000.0f, "%.2f");
    
    rectTransformPtr->getLocalRotationModifiable() = glm::radians(rotation);
}

void EditorModule::drawPaddle(Paddle* paddlePtr)
{
    ImGui::DragFloat3( "Max postition", (float*)&paddlePtr->maxPos, 0.05f, -20.0f, 20.0f, "%.2f");
    ImGui::DragFloat("Min speed", (float*)&paddlePtr->minSpeed, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Max speed", (float*)&paddlePtr->maxSpeed, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Max front rotation ", (float*)&paddlePtr->maxFrontRot, 0.5f, -90.0f, 90.0f);
    ImGui::DragFloat("Max side rotation ", (float*)&paddlePtr->maxSideRot, 0.5f, -90.0f, 90.0f);
}

void EditorModule::drawBone(Bone* bonePtr)
{
    ImGui::Text(("Bone Name: " + bonePtr->boneName).c_str());
    ImGui::SameLine();
    ImGui::Text(("Bone ID: " + std::to_string(bonePtr->boneID)).c_str());
}

void EditorModule::drawLight(Light* lightPtr)
{
    ImGui::ColorEdit3("Color ", (float*)&lightPtr->color);
    ImGui::DragFloat("Intensity: ", &lightPtr->intensity, 1.0f, 0.0f, 100.0f, "%.2f");
}

void EditorModule::drawRigidbody(Rigidbody* rBodyPtr)
{
    ImGui::Checkbox("Ignore Gravity", &rBodyPtr->ignoreGravity);
    if( ImGui::DragFloat("Mass", &rBodyPtr->mass) )
    {
        if (auto box = rBodyPtr->entityPtr->getComponentPtr<BoxCollider>())
        {
            glm::mat3 I = BoxMomentOfInertia(rBodyPtr->mass, box->halfSize * 2.0f);
            rBodyPtr->momentOfInertia = I;
            rBodyPtr->invertedMomentOfInertia = glm::inverse(I);
        }
        else if (auto sphere = rBodyPtr->entityPtr->getComponentPtr<SphereCollider>())
        {
            //TODO implement solid or hollow
            glm::mat3 I = SphereMomentOfInertia(rBodyPtr->mass, sphere->radius);
            rBodyPtr->momentOfInertia = I;
            rBodyPtr->invertedMomentOfInertia = glm::inverse(I);
        }
    }
    ImGui::DragFloat("Drag", &rBodyPtr->drag);
    ImGui::DragFloat("Angular drag", &rBodyPtr->angularDrag);
    ImGui::Text((std::string("Velocity: ") + formatVec3(rBodyPtr->velocity)).c_str());
    ImGui::Text((std::string("Angular velocity: ") + formatVec3(rBodyPtr->angularVelocity)).c_str());
}

void EditorModule::sortEntities(SortingType sortingType)
{
    entities.clear();

    int entitiesSize = (*objectModule->getEntitiesVector()).size();

    for(int i = 0; i < entitiesSize; ++i)
    {
        Entity* temp = objectModule->getEntityPtrByID(i);
        switch(sortingType)
        {
            case SortingType::TRANSFORM_WITHOUT_BONES:
                if(temp->getComponentPtr<Bone>() != nullptr)
                {
                    continue;
                }
            case SortingType::TRANSFORM:
                if(temp->getComponentPtr<Transform>() == nullptr)
                {
                    continue;
                }
            break;
            case SortingType::BONES:
                if(temp->getComponentPtr<Bone>() == nullptr)
                {
                    continue;
                }
            break;
            case SortingType::PADDLE:
                if(temp->getComponentPtr<Paddle>() == nullptr)
                {
                    continue;
                }
            break;
            case SortingType::RIGIDBODIES:
                if(temp->getComponentPtr<Rigidbody>() == nullptr)
                {
                    continue;
                }
            break;
        }
        insertEntityToList(i);
    }

    if(entities.size() == 0)
    {
        for(int i = 0; i < entitiesSize; ++i)
        {
            insertEntityToList(i);
            enumValue = 0;
        }
    }
}

unsigned int EditorModule::getEntityIdFromCombo(int chosenField)
{
    int id = 0;
    std::string temp = entities;
    while(id != chosenField)
    {
        temp = temp.substr(temp.find_first_of(char(0)) + 1);
        id++;
    }
    size_t begPos = temp.find_first_of(" ");
    size_t endPos = temp.find_first_of("N") - 1;
    std::string idString = temp.substr(begPos + 1, endPos - begPos);
    return std::stoi(idString);
}

void EditorModule::insertEntityToList(unsigned int id)
{
    entities += "ID ";
    entities += std::to_string(id);
    entities += " Name: ";
    entities += objectModule->getEntityPtrByID(id)->getName();
    entities += char(0);
}