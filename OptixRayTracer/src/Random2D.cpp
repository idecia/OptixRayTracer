#include "Random2D.h"


Random2D::Random2D(RNG *rng, uint32_t totalSamples)
	: rng(rng), totalSamples(totalSamples), nGenerated(0) {

}

RT_HOSTDEVICE bool Random2D::Next2D(float2 *sample) {

	sample->x = rng->RandomFloat();
	sample->y = rng->RandomFloat();
	nGenerated++;
	return (nGenerated < totalSamples);
}