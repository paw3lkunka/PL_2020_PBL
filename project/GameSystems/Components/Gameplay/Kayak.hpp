#ifndef _PLAYER_INL
#define _PLAYER_INL

#include "Component.hpp"

class Transform;

/**
 * @brief This structure stores current state of player current state.
 * IMPORTANT: By now this component shouldn't be moved between entities.
 * IMPORTANT: There should be just one instance of this component at once.
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

    /**
     * @brief Construct a new Kayak object.
     * Set last created entity to this.
     */
    Kayak();

    /**
     * @brief Destroy the Kayak object.
     * HACK: Set last created instance to null, even if other instance exist.
     */
    ~Kayak();

    /**
     * @brief Getter to last created instance of kayak.
     * @return pointer to instance (may be nullptr, if instance doesn't exist).
     */
    static Kayak* get();

    /**
     * @brief Getter to transform of entity owning last created kayak component.
     * @return pointer to transform component (may be nullptr, if instance doesn't exist).
     * HACK: Will fail, if kayak was assigned to diffrent entity in runtime.
     */
    static Transform* getTransform();

private:
    static Kayak* lastInstance;
	static Transform* kayakTransformPtr;
};

#endif

