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
};

#endif /* !CARGOSTORAGE_INL_ */
