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
        CargoType("Food", false, {3, 15}, {100, 500}),
        CargoType("Letters", true, {0.5, 2}, {50, 600}),
        CargoType("Alcohol", false, {5, 15}, {200, 1100}),
        CargoType("Cigarettes", false, {0.4, 3}, {250, 500}),
        CargoType("Medicines", false, {0.5, 4}, {100, 600}),
        CargoType("Weapon", true, {15, 70}, {300, 2500}),
        CargoType("Jewelry", false, {5, 10}, {300, 1000}),
        CargoType("Fake Documents", true, {0.8, 7}, {600, 2000})
    };
    
    const int baseCargosSize = 8;
}

#endif /* !CARGODATASTRUCTURES_INL_ */
