#pragma once

#include <curand_kernel.h>
#include <optixu/optixu_math_stream_namespace.h>
using namespace optix;

class RNG {

public:

	RNG(uint32_t seed, uint32_t index);
	RT_HOSTDEVICE float RandomFloat();
	RT_HOSTDEVICE uint32_t RandomUInt();
	RT_HOSTDEVICE uint32_t RandomUInt(uint32_t bound);

private:

	curandState rngState;

};

