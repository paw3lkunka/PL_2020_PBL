#ifndef CARGO_INL_
#define CARGO_INL_

#include "Component.hpp"
#include <string>
#include <cstring>
#include <cmath>

/**
 * @brief kayak cargo component
 */
struct Cargo : public Component
{
    Cargo() = default;
    ~Cargo() = default;

    ///@brief name of cargo type
    std::string name;
    ///@brief is cargo risky flag
    bool isRisky = false;
    ///@brief weight of cargo (in kg?)
    float weight;
    ///@brief income of cargo 
    float income;

    /**
     * @brief comparsion operator (equality)
     * 
     * @param other cargo to compare
     * @return true if cargos are equal
     * @return false  if cragos ar inequal
     */
    bool operator == (Cargo& other)
    {
        return (strcmp(this->name.c_str(), other.name.c_str()) == 0) 
            && (this->isRisky == other.isRisky) 
            && fabs(this->weight - other.weight) < 0.001f
            && fabs(this->income - other.income) < 0.001f;
    }

    /**
     * @brief comparsion operator (inequality)
     * 
     * @param other cargo to compare
     * @return true if cargos are inequal
     * @return false if cargos are equal
     */
    bool operator != (Cargo& other)
    {
        return !(*this == other);
    }
};

#endif /* !CARGO_INL_ */
