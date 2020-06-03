#include "Kayak.hpp"

Kayak* Kayak::lastInstance = nullptr;

Kayak::Kayak()
{
    lastInstance = this;
}

Kayak::~Kayak()
{
    lastInstance = nullptr;
}

Kayak* Kayak::get()
{
    return lastInstance;
} 