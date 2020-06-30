#ifndef CARGOSTORAGE_INL_
#define CARGOSTORAGE_INL_

#include "Component.hpp"
#include <vector>

struct Cargo;

//TODO: documentation
struct CargoStorage : public Component
{
public:
    CargoStorage() = default;
    ~CargoStorage() = default;

//TODO: documentation
    std::vector<Cargo*> cargosStored;
//TODO: documentation
    std::vector<Cargo*> cargosLost;

//TODO: documentation
    float weightLimit = 70.0f;

//TODO: documentation
    float weightSum = 0.0f;
//TODO: documentation
    float incomeSum = 0.0f;
};

#endif /* !CARGOSTORAGE_INL_ */
