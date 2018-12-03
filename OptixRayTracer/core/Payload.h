#pragma once

#include "core/optix_global.h"
#include "samplers/RNG.h"

struct RadiancePayload {

   RNG rng;
   float3 color;
   int depth;

};

struct ShadowPayload {

   float attenuation;

   
};

struct ReinhartPayload {
	
	RNG rng;
	int depth;
	int patchID;
	float3 value;
	//double3 value;
};

struct BeckersPayload {

	RNG rng;
	int depth;
	int dirID;
	float3 value;
	//double3 value;
};


