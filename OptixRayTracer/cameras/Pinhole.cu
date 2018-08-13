
#include <optix_world.h>

#include "core/optix_global.h"
#include "core/Ray.h"
#include "core/Payload.h"
#include "cameras/Pinhole.h"
#include "films/Film.h"
#include "core/RNG.h"
#include "samplers/Random2D.h"
#include <optix_device.h>

rtBuffer<RNG, 2> rngs;
rtDeclareVariable(uint2, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(Pinhole, camera, , );
rtDeclareVariable(Film, film, , );
rtDeclareVariable(rtObject, root, , ); 


RT_PROGRAM void pinhole(void) {
	int nSamples = 1000;
	

	RadiancePayload radiancePayload;
	radiancePayload.rng = rngs[pixelIdx];

	Random2D sampler(&radiancePayload.rng, nSamples);
	float2 uniformSample;

	float3 result = make_float3(0.0f);

	while (sampler.Next2D(&uniformSample)) {
		
		radiancePayload.color = make_float3(0.0f);
		radiancePayload.depth = 0;

		float2 filmSample = film.Sample(pixelIdx, uniformSample);
		//rtPrintf("(%d, %d)   ", pixelIdx.x, pixelIdx.y);
		//rtPrintf("(%f, %f)   ", uniformSample.x, uniformSample.y);
		//rtPrintf("(%f, %f)\n", filmSample.x, filmSample.y);
		Ray ray = camera.GenerateRay(filmSample);
		rtTrace(root, ray, radiancePayload);
		//rtPrintf("(%f, %f, %f)\n", radiancePayload.color.x, radiancePayload.color.y, radiancePayload.color.z);
		result += radiancePayload.color;

	}

	film.PutSample(pixelIdx, result / (float)nSamples);
	rngs[pixelIdx] = radiancePayload.rng;


}


RT_PROGRAM void exception() {

	rtPrintExceptionDetails();

}
