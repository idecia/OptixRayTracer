#pragma once

#include "core/optix_global.h"

enum RayType {
   RADIANCE,
   SHADOW,
   RAY_TYPE_COUNT,
};

enum RayTypeOpt {
	REINHART_RADIANCE,
	REINHART_SHADOW,
	BECKERS_RADIANCE,
	BECKERS_SHADOW,
};


struct HitRecord {
   float3 position;
   float3 normal;
};


