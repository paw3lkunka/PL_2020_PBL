#ifndef EDITORMODULE_HPP_
#define EDITORMODULE_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <glm/glm.hpp>

#include "ComponentsPreDeclarations.hxx"
#include "EditorDataStructures.inl"

class Entity;
class ObjectModule;
class Texture;
class Shader;
class Font;

/**
 * @brief Simple editor for our purposes
 */
class EditorModule
{
public:
    EditorModule() = default;
    ~EditorModule() = default;

    /**
     * @brief initialize ImGui - call in Core init
     * 
     * @param window pointer to window created by glfw
     */
    void init(GLFWwindow* window);

    /**
     * @brief Setup some variables for later usage - call before game loop
     */
    void setup();

    /**
     * @brief Make ImGui window editor - call in frame update BEFORE renderer
     */
    void drawEditor();

    /**
     * @brief cleanup ImGui components - call in Core cleanup
     */
    void onExit();

private:
    ///@brief object module pointer - easier to use than GetCore().objectModule
    ObjectModule* objectModule;

    ///@brief pointer for actual entity
    Entity* entityPtr;

    ///@brief list of entities names with \0 in between
    std::string entities;
    ///@brief index for current element in combo list
    int currentItem;

    //HACK: Maybe other solution?
    ///@brief list of sorting types, according to enum class (the same order as in enum class)
    const char* sortingTypesList = "All\0Transform\0Transform without bones\0Paddle\0Bones\0Rigidbodies\0Kayak\0Enemy\0RectTransform\0";
    ///@brief index of current sorting type
    int enumValue;

    // ! UI Maker methods
    void drawMaker();
    void makeNewButton(glm::vec2 pos, glm::vec2 size);
    void makeNewText(glm::vec2 pos, std::string text);
    Texture* buttonSprite;
    Shader* buttonShader;
    Entity* positionPointer;
    Font* fontPtr;
    Shader* textShader;
    int lastEntitySize;

    // ! Component drawing functions
    void drawTransform(Transform* transformPtr);
    void drawRectTransform(RectTransform* rectTransformPtr);
    void drawPaddle(Paddle* paddlePtr);
    void drawBone(Bone* bonePtr);
    void drawLight(Light* lightPtr);
    void drawRigidbody(Rigidbody* rBodyPtr);
    void drawKayak(Kayak* kayakPtr);
    void drawEnemy(Enemy* enemyPtr);
    void drawCamera(Camera* camera);
    void drawEnemyAttack(EnemyAttack* attackPtr);
    void drawButton(Button* button);
    void drawText(TextRenderer* textRenderer);
    void drawEnemyAnimation(EnemyAnimation* enemyAnimationPtr);
    void drawSortingGroup(UiSortingGroup* sortingGroupPtr);
    void drawToggleButton(ToggleButton* toggleButtonPtr);
    void drawProgressBar(ProgressBar* progressBarPtr);

    // ! Helper functions
    /**
     * @brief Formating glm::vector 3 to precision 2 floats
     * 
     * @param vector vector to format
     * @return std::string string representation of vector, with formatted coordinates
     */
    std::string formatVec3(glm::vec3 vector);

    /**
     * @brief Sorting entities for sorting types
     * 
     * @param sortingType type of sorting
     */
    void sortEntities(SortingType sortingType = SortingType::ALL);

    /**
     * @brief Get the Entity Id From ImGui Combo object
     * 
     * @param chosenField index from Combo
     * @return unsigned int entity id
     */
    unsigned int getEntityIdFromCombo(int chosenField);

    /**
     * @brief insert entity to entity combo list
     * 
     * @param id entity id to insert
     */
    void insertEntityToList(Entity* entity);
};

#endif /* !EDITORMODULE_HPP_ */
