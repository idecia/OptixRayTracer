#pragma once

#include "core/optix_global.h"

enum RayType {
	RADIANCE,
	SHADOW,
	RAY_TYPE_COUNT
};


struct HitRecord {
	float3 position;
	float3 normal;
};


