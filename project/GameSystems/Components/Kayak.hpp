#ifndef _PLAYER_INL
#define _PLAYER_INL

#include "Component.hpp"

class Transform;

/**
 * @brief This structure stores current state of player current state.
 */
struct Kayak : public Component
{
    /**
     * @brief Store number of activated hideouts, can be interpreted as bool.
     */
    int isHidden = 0;

    /**
     * @brief Store number of informed enemies, can be interpreted as bool; 
     */
    int isDetected = 0;

    Kayak();
    ~Kayak();
    static Kayak* get();
    static Transform* getTransform();
private:
    static Kayak* lastInstance;
	static Transform* kayakTransformPtr;
};

#endif

