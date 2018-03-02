#pragma once

#include "RNG"
#include <optixu/optixu_math_stream_namespace.h>
using namespace optix;

class Random2D {

public:

	Random2D(RNG *rng, uint32_t totalSamples);
	RT_HOSTDEVICE bool Next2D(float2 *sample); 


private:

	RNG rng;
	uint32_t totalSamples;
	unint32_t nSamples;
};

