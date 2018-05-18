
#include <optix_world.h>

#include "core/optix_global.h"
#include "core/Ray.h"
#include "core/Payload.h"
#include "samplers/Util.h"
#include "core/RNG.h"
#include "core/ONB.h"
#include "samplers/Random2D.h"
#include <optix_device.h>

rtBuffer<RNG> rngs;
//rtBuffer<float2> u;
rtBuffer<float3> coeff;
rtBuffer<unsigned int> ns;
rtDeclareVariable(int, N, ,);
rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(float3, sensorNormal, , );
rtDeclareVariable(float3, sensorPos, , );
rtDeclareVariable(rtObject, root, , );
rtDeclareVariable(unsigned int, NskyPatches, , );

RT_PROGRAM void sensor(void) {

	int nSamples = 500;
	ReinhartPayload pl;
	pl.rng = rngs[pixelIdx];

	Random2D sampler(&pl.rng, nSamples);
	float2 unifSample; //= u[pixelIdx];

	while (sampler.Next2D(&unifSample)) {

		pl.value = make_float3(0.0f);
		pl.depth = 0;


		ONB onb(sensorNormal);
		float3 dir   = UniformHemisphereSample(unifSample.x, unifSample.y);
		float3 dirW  = onb.LocalToWorld(dir);
		Ray ray = make_Ray(sensorPos, dirW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
		//rtPrintf("%f %f %f\n", dirW.x, dirW.y, dirW.z);

		rtTrace(root, ray, pl);
		
		if (fmaxf(pl.value) > 0.0f) {
			float3 value = pl.value  ;
			//rtPrintf("%f %f\n", coeff[pl.patchID].x, value.x);
			atomicAdd(&coeff[pl.patchID].x, value.x);
			atomicAdd(&coeff[pl.patchID].y, value.y);
		    atomicAdd(&coeff[pl.patchID].z, value.z);
			//ns[pl.patchID]++;
		}
		
		
	}
	/*for (int i = 0; i < NskyPatches; i++) {
		coeff[i].x /= ns[i];
		coeff[i].y /= ns[i];
		coeff[i].z /= ns[i];
		coeff[i] = coeff[i] * M_PI;
		//rtPrintf("%d %f\n", ns[i], coeff[i].x);
	}*/
	rngs[pixelIdx] = pl.rng;

}


RT_PROGRAM void exception() {

	rtPrintExceptionDetails();

}
