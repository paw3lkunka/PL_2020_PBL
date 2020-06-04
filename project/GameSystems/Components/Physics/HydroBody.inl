#ifndef _HYDROBODY_INL
#define _HYDROBODY_INL

#include "Component.hpp"
#include "HydroDataStructures.hpp"
#include "HydroSurface.inl"

struct HydroBody : public Component
{
    HydroBody() = default;
    ~HydroBody() = default;

    HydroSurface* hydroSurface = nullptr;

    // TODO: Bring slamming force back to life
    //std::vector<HydroSlammingForceData> slammingForceData;
};

#endif // _HYDROBODY_INL
