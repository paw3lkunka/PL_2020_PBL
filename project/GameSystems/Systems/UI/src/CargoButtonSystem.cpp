#include "CargoButtonSystem.hpp"

#include "Entity.hpp"
#include "CargoButton.inl"
#include "CargoDataStructures.inl"
#include "Core.hpp"
#include "Texture.hpp"

void CargoButtonSystem::init()
{
    TextureCreateInfo info = {};
    info.format = GL_RGBA;
    info.generateMipmaps = false;
    info.magFilter = GL_LINEAR;
    info.minFilter = GL_LINEAR;
    info.wrapMode = GL_CLAMP_TO_EDGE;

    stampSprite = GetCore().objectModule.newTexture("Resources/Sprites/penaltyStamp.png", info);
    auto sh = GetCore().objectModule.getShaderPtrByName("UiStandard");
    stampMaterial = GetCore().objectModule.newMaterial(sh, "CargoRiskyMat", RenderType::Transparent);
    stampMaterial->setTexture("sprite", stampSprite);
    stampMaterial->setVec4("color", {1.0f, 1.0f, 1.0f, 0.7f});
}

bool CargoButtonSystem::assertEntity(Entity* entity)
{
    cargoButtonPtr = entity->getComponentPtr<CargoButton>();
    return cargoButtonPtr != nullptr;
}

void CargoButtonSystem::start()
{
    CargoType data = Cargos::baseCargos[GetCore().randomInt(Cargos::baseCargosSize - 1)];
    Cargo* cargo = GetCore().objectModule.newEmptyComponent<Cargo>();
    cargo->name = data.name;
    cargo->isRisky = data.isRisky;
    cargo->income = GetCore().randomFloatR(data.incomeRange.x, data.incomeRange.y);
    cargo->weight = GetCore().randomFloatR(data.weightRange.x, data.weightRange.y);
    cargoButtonPtr->entityPtr->addComponent(cargo);
    cargoButtonPtr->onActivateEvents.push_back(Message(Event::ADD_CARGO, cargo));
    cargoButtonPtr->onDeactivateEvents.push_back(Message(Event::REMOVE_CARGO, cargo));
    

    cargoButtonPtr->nameText->mesh.text = cargo->name;
    if(cargo->isRisky)
    {
        auto ent = GetCore().objectModule.newEntity(2, Name(cargo) + "_RiskyStamp");
        auto rect = GetCore().objectModule.newEmptyComponentForLastEntity<RectTransform>();
        rect->setParent(cargo->entityPtr->getComponentPtr<RectTransform>());
        rect->getLocalPositionModifiable() = {-129.0f + (4.0f - GetCore().randomFloatL(8.0f)), 
                                                57.0f + (4.5f - GetCore().randomFloatL(9.0f))};

        rect->getLocalRotationModifiable() = glm::radians(-20.0f + (10 - GetCore().randomFloatL(20.0f)));

        rect->getSizeModifiable() = {120, 120};
        auto render = GetCore().objectModule.newEmptyComponentForLastEntity<UiRenderer>();
            render->material = stampMaterial;
    }

    cargoButtonPtr->incomeText->mesh.text = getFloatWithPrecision(cargo->income, 2) + "$";
    cargoButtonPtr->weightText->mesh.text = getFloatWithPrecision(cargo->weight, 2) + "kg";
}