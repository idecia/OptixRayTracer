
#include <optix_world.h>

#include "core/optix_global.h"
#include "core/Ray.h"
#include "core/Payload.h"
#include "samplers/Util.h"
#include "samplers/RNG.h"
#include "core/ONB.h"
#include "core/math.h"
#include "skyes/Beckers288.h"
#include "samplers/Random2D.h"
#include <optix_device.h>

rtBuffer<RNG> rngs;
rtBuffer<float3, 2> env ;
rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(float3, sensorNormal, , );
rtDeclareVariable(float3, sensorPos, , );
rtDeclareVariable(rtObject, root, , );
rtDeclareVariable(int, Ntot, , );
rtDeclareVariable(int, nSamples , , );

RT_PROGRAM void sensor(void) {
	//rtPrintf("ok");
	ReinhartPayload pl;
	pl.rng = rngs[pixelIdx];

	Random2D sampler(&pl.rng, nSamples);
	float2 unifSample; 

	while (sampler.Next2D(&unifSample)) {
		pl.value = make_float3(0.0f);
		pl.depth = 0;

		ONB onb(sensorNormal);
		float3 dir = UniformHemisphereSample(unifSample.x, unifSample.y);
		float3 dirW = onb.LocalToWorld(dir);
		Ray ray = make_Ray(sensorPos, dirW, RayTypeOpt::REINHART_RADIANCE, 0, RT_DEFAULT_MAX);
		rtTrace(root, ray, pl);
		//rtPrintf("%f %f %f %f %f %f\n", dir.x, dir.y, dir.z, sensorPos.x, sensorPos.y, sensorPos.z);
		if (fmaxf(pl.value) > 0.0) {
			uint2 index;
			index.x = beckers(dir);
			index.y = pl.patchID;
			//if (index.x == 242) {
				//rtPrintf("- %f %f %f %f %d %d\n", dir.x, dir.y, dir.z, env[index].x, beckers(dir), index.y);
			//}
			//ncell = 288; AREGLAR ESTO QUE ESTA HARCODEADO!
			//float3 value = (288/2*M_PIf) * 2*M_PIf*pl.value/Ntot;
			float3 value = (288 * pl.value) / Ntot;
			//float3 value =pl.value ;
			atomicAdd(&env[index].x, (float)value.x);
			atomicAdd(&env[index].y, (float)value.y);
			atomicAdd(&env[index].z, (float)value.z); // falta multiplicar por 2pi/N
		}
		



	}
	rngs[pixelIdx] = pl.rng;

	/*for (int i = 0; i < 288; i++)  {
		for (int j = 0; j < 146; j++) {
			uint2 index;
			index.x = i;
			index.y = j;
			env[index] = make_float3(j);
			//rtPrintf("%f    ", v.x);
		}
		//rtPrintf("\n");
	}*/

}


RT_PROGRAM void exception() {

	rtPrintExceptionDetails();

}
