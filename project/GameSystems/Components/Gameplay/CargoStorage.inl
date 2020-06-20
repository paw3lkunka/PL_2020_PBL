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
    int cargosStoredSize;

    float weightLimit = 30.0f;

    float weightSum = 0.0f;
    float incomeSum = 0.0f;
};

#endif /* !CARGOSTORAGE_INL_ */
