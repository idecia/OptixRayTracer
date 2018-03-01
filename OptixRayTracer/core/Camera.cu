#include "optix_global.h"
#include "Camera.h"
#include "Film.h"


using namespace optix;

rtBuffer<float3, 2>   outputBuffer;
rtBuffer<RNG, 2> rngs;
rtDeclareVariable(uint2, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(Camera, camera, , );
rtDeclareVariable(Film, film, , );
rtDeclareVariable(rtObject, root, , );

struct Payload {

	RNG rng;
	float3 color;

};

RT_PROGRAM void pinhole(void) {

	Payload payload;
	payload.rng = rngs[pixelIdx];
	Random2D sampler(&payload.rng, 1u);
	float2 uniformSample;
	while (sampler.Next2D(&uniformSample)) {
		float2 filmSample = film.Sample(pixelIdx, uniformSample);
		Ray ray = camera.GenerateRay(filmSample);
		rtTrace(root, ray, payload);
		film.PutSample(filmSample, payload.color)
	}
	rngs[pixelIdx] = payload.rng;

}