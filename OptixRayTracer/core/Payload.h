#include "core/optix_global.h"

struct RadiancePayload {

	RNG rng;
	float3 color;
	int depth;

};

struct ShadowPayload {

	float attenuation;

	
};

