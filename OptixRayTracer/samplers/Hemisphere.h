#pragma once

#include "core/RNG.h"
#include "core/optix_global.h"

class Hemisphere {

public:

	RT_FUNCTION Hemisphere();
	RT_FUNCTION Hemisphere(RNG *rng, uint32_t totalSamples);
	RT_FUNCTION bool Next3D(float3 *sample);


private:

	RNG* rng;
	uint32_t totalSamples;
	uint32_t generatedSamples;
};


RT_FUNCTION Hemisphere::Hemisphere() {

}


RT_FUNCTION Hemisphere::Hemisphere(RNG *rng, uint32_t totalSamples)
	: rng(rng), totalSamples(totalSamples), generatedSamples(0) {


}

RT_FUNCTION bool Hemisphere::Next3D(float3 *sample) {

	float x = rng->RandomFloat();
	float y = rng->RandomFloat();
	float cosPhi = cosf(2.0f * M_PIf * x);
	float sinPhi = sinf(2.0f * M_PIf * x);
	float cosTheta = 1.0f - y;
	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	sample->x = sinTheta * cosPhi;
	sample->y = sinTheta * sinPhi;
	sample->z = cosTheta;

	++generatedSamples;
	return (generatedSamples <= totalSamples);

}


