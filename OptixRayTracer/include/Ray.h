#pragma once

#include <optixu/optixu_math_namespace.h>
using namespace optix;

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


