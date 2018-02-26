#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include "Camera.h"
#include "Film.h"


using namespace optix;

rtBuffer<float3, 2>   outputBuffer;
rtBuffer<RNG, 2> rngs;
rtDeclareVariable(uint2, idx, rtLaunchIndex, );
rtDeclareVariable(Camera, camera, , );
rtDeclareVariable(Film, film, , );
rtDeclareVariable(rtObject, root, , );

struct Payload {

	RNG rng;
	float3 color;

};

RT_PROGRAM void pinhole(void) {
	Payload payload;
	payload.rng = rngs[idx];
	Random2D sampler(&payload.rng, 1u);
	float2 sample;
	while (sampler.Next2D(&sample)) {
		float2 filmSample = film.Sample(idx, sample)
		Ray ray = camera.GenerateRay(filmSample);
		rtTrace(root, ray, payload);
		film.PutSample(filmSample, payload.color)
	}
	rngs[index] = payload.rng;

}