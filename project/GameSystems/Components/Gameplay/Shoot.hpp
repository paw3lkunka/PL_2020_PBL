#ifndef Shoot_inl_sh
#define Shoot_inl_sh

#include "Component.hpp"

/**
 * @brief Object of a bullet trail
 * HACK: should be on preinitialized on all enemies scene; 
 */
struct Shoot : public Component
{
    inline Shoot() { instance = this; }
    inline virtual ~Shoot() { instance = nullptr; }

    /**
     * @brief Global getter of Shoot instance.
     * 
     * @return Shoot& 
     */
    static Shoot& get() { return *instance; }
private:
    static Shoot* instance;
};

#endif
