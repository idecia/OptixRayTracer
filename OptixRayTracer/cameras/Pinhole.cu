
#include <optix_world.h>

#include "core/optix_global.h"
#include "core/Ray.h"
#include "core/Payload.h"
#include "cameras/Pinhole.h"
#include "films/Film.h"
#include "core/RNG.h"
#include "samplers/Random2D.h"
using namespace optix;

rtBuffer<RNG, 2> rngs;
rtDeclareVariable(uint2, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(Pinhole, camera, , );
rtDeclareVariable(Film, film, , );
rtDeclareVariable(rtObject, root, , ); 



RT_PROGRAM void pinhole(void) {

	RadiancePayload radiancePayload;
	radiancePayload.color = make_float3(0.0f);
	radiancePayload.depth = 0;
	radiancePayload.rng = rngs[pixelIdx];

	Random2D sampler(&radiancePayload.rng, 1u);
	float2 uniformSample;

	while (sampler.Next2D(&uniformSample)) {

		float2 filmSample = film.Sample(pixelIdx, uniformSample);
		Ray ray = camera.GenerateRay(filmSample);
		rtTrace(root, ray, radiancePayload);
		film.PutSample(filmSample, radiancePayload.color);

	}

	rngs[pixelIdx] = radiancePayload.rng;

}