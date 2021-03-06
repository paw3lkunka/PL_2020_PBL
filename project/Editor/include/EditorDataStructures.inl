#ifndef EDITORDATASTRUCTURES_INL_
#define EDITORDATASTRUCTURES_INL_

/**
 * @brief Enum class for sorting entity by component types
 */
enum class SortingType : unsigned int
{
    ALL = 0,
    TRANSFORM,
    TRANSFORM_WITHOUT_BONES,
    PADDLE,
    BONES,
    RIGIDBODIES,
    KAYAK,
    ENEMY,
    RECT_TRANSFORM,
    HYDRO_CURRENT,
    FINISH,
};

#endif /* !EDITORDATASTRUCTURES_INL_ */
