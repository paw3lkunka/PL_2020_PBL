#ifndef CARGODATASTRUCTURES_INL_
#define CARGODATASTRUCTURES_INL_

#include <glm/vec2.hpp>
#include <string>

/**
 * @brief Struct of cargo type - information for generating cargo components
 */
struct CargoType
{
    ///@brief name of cargo type
    std::string name;
    ///@brief is cargo risky flag
    bool isRisky = false;
    ///@brief generator info: range of weight; x is min, y is max
    glm::vec2 weightRange;
    ///@brief generator info: range of income; x is min, y is max
    glm::vec2 incomeRange;

    CargoType(std::string n, bool iR, glm::vec2 wR, glm::vec2 inR) : name(n), isRisky(iR), weightRange(wR), incomeRange(inR) {}
};

namespace Cargos
{
    const CargoType baseCargos[] =
    {
        CargoType("Food", false, {3, 15}, {20, 100}),
        CargoType("Letters", true, {0.5, 2}, {10, 120}),
        CargoType("Alcohol", false, {5, 15}, {30, 200}),
        CargoType("Cigarettes", false, {0.4, 3}, {10, 150}),
        CargoType("Medicines", false, {0.5, 4}, {25, 125}),
        CargoType("Weapon", true, {15, 70}, {40, 300}),
        CargoType("Jewelry", false, {5, 10}, {25, 250}),
        CargoType("Fake Documents", true, {0.8, 7}, {15, 160})
    };
    
    const int baseCargosSize = 8;
}

#endif /* !CARGODATASTRUCTURES_INL_ */
