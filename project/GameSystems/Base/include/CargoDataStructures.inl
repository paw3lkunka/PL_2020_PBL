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
        CargoType("Food", false, {0.2, 10}, {100, 500}),
        CargoType("Letters", false, {0.1, 1}, {50, 250}),
        CargoType("Alcohol", false, {3, 15}, {200, 1100}),
        CargoType("Cigarettes", false, {0.2, 2}, {250, 500}),
        CargoType("Medicines", false, {0.3, 4}, {100, 400}),
        CargoType("Weapon", true, {10, 50}, {300, 2000}),
        CargoType("Jewelry", false, {2, 8}, {300, 1000}),
        CargoType("Fake Documents", true, {0.3, 5}, {600, 1500})
    };
}

#endif /* !CARGODATASTRUCTURES_INL_ */
