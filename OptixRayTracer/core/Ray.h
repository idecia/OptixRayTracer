#pragma once

#include "core/optix_global.h"

enum RayType {
   RADIANCE,
   SHADOW,
   REINHART_RADIANCE,
   REINHART_SHADOW,
   BECKERS_RADIANCE,
   BECKERS_SHADOW,
   RAY_TYPE_COUNT
};


struct HitRecord {
   float3 position;
   float3 normal;
};


