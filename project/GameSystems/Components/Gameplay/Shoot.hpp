#ifndef Shoot_inl_sh
#define Shoot_inl_sh

#include "Component.hpp"

//TODO documentation
struct Shoot : public Component
{
    inline Shoot() { instance = this; }
    inline virtual ~Shoot() { instance = nullptr; }

    static Shoot& get() { return *instance; }
private:
    static Shoot* instance;
};

#endif
