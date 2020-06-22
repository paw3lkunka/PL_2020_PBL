#ifndef CARGOSTORAGE_INL_
#define CARGOSTORAGE_INL_

#include "Component.hpp"
#include <list>

struct Cargo;

struct CargoStorage : public Component
{
public:
    CargoStorage() = default;
    ~CargoStorage() = default;

    std::list<Cargo*> cargosStored;
    std::list<Cargo*> cargosLost;
    int cargosStoredSize;

    float weightLimit = 70.0f;

    float weightSum = 0.0f;
    float incomeSum = 0.0f;
};

#endif /* !CARGOSTORAGE_INL_ */
