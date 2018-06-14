
#include <optix_world.h>

#include "core/optix_global.h"
#include "core/Ray.h"
#include "core/Payload.h"
#include "samplers/Util.h"
#include "core/RNG.h"
#include "core/ONB.h"
#include "core/math.h"
#include "skyes/Beckers288.h"
#include "samplers/Random2D.h"
#include <optix_device.h>

rtBuffer<RNG> rngs;
rtBuffer<float3, 2> coeff ;
rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(float3, sensorNormal, , );
rtDeclareVariable(float3, sensorPos, , );
rtDeclareVariable(rtObject, root, , );

RT_PROGRAM void sensor(void) {

	int nSamples = 1000;
	ReinhartPayload pl;
	pl.rng = rngs[pixelIdx];

	Random2D sampler(&pl.rng, nSamples);
	float2 unifSample; 

	while (sampler.Next2D(&unifSample)) {
		pl.value = make_float3(0.0f);
		pl.depth = 0;

		ONB onb(sensorNormal);
		float3 dir  = UniformHemisphereSample(unifSample.x, unifSample.y);
		float3 dirW = onb.LocalToWorld(dir);
		Ray ray = make_Ray(sensorPos, dirW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
		rtTrace(root, ray, pl);

		if (fmaxf(pl.value) > 0.0) {
			uint2 index;
			index.x = beckers(dir);
			index.y = pl.patchID;
			float3 value = pl.value;
			atomicAdd(&coeff[index].x, (float)value.x);
			atomicAdd(&coeff[index].y, (float)value.y);
			atomicAdd(&coeff[index].z, (float)value.z); // falta multiplicar por 2pi/N
		}


	}
	rngs[pixelIdx] = pl.rng;

}


RT_PROGRAM void exception() {

	rtPrintExceptionDetails();

}
