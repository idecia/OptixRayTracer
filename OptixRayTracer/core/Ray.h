#pragma once

#include "core/optix_global.h"

enum RayType {
	RADIANCE,
	SHADOW
};

struct RadianceRayPayload {
	float3 color;
	int recursionDepth;
};

struct ShadowRayPayload {
	float3 color;
	int recursionDepth;
};

struct HitRecord {
	float3 position;
	float3 normal;
};


