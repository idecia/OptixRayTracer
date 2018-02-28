#pragma once

#include "core/optix_global.h"

enum RayType {
	RADIANCE_RAY,
	SHADOW_RAY
};

struct RadianceRayPayload {
	float3 color;
	int recursionDepth;
};

struct ShadowRayPayload {
	float3 color;
	int recursionDepth;
};


