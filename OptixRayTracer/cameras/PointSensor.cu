
#include <optix_world.h>

#include "core/optix_global.h"
#include "core/Ray.h"
#include "core/Payload.h"
#include "samplers/Util.h"
#include "samplers/RNG.h"
#include "core/ONB.h"
#include "core/math.h"
#include "samplers/Random2D.h"
#include <optix_device.h>

rtBuffer<RNG> rngs;
//rtBuffer<float2> u;
//rtBuffer<float3> coeff;
rtBuffer<float> coeff;
//rtBuffer<double3> coeff;
rtDeclareVariable(double, out, , );
rtDeclareVariable(int, Ntot, ,);
rtDeclareVariable(int, nSamples, , );
rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(float3, sensorNormal, , );
rtDeclareVariable(float3, point, , );
rtDeclareVariable(float3, sensorPos, , );
rtDeclareVariable(rtObject, root, , );
rtDeclareVariable(unsigned int, NskyPatches, , );


RT_PROGRAM void sensor(void) {

	ReinhartPayload pl;
	pl.rng = rngs[pixelIdx];

	Random2D sampler(&pl.rng, nSamples);
	float2 unifSample; //= u[pixelIdx];


	while (sampler.Next2D(&unifSample)) {

		//pl.value = make_float3(0.0f);
		pl.value = make_float3(0.0f);
		pl.depth = 0;


		ONB onb(sensorNormal);
		float3 dir   = CosineHemisphereSample(unifSample.x, unifSample.y);
		float3 dirW  = onb.LocalToWorld(dir);
		//float3 sensorPos2 = make_float3(1.118916, 4.990000, 3.026103);
		//float3 sensorPos2 = make_float3(-1.444052f, -0.000000f, 3.0005f);
		//float3 sensorPos2 = make_float3(-1.490000f, -0.540149f, 0.997289f);
		//dirW = make_float3(0.945326f, 0.091734f, -0.312960f);
	  //  dirW = make_float3( -0.456876, -0.88952, -0.004548);
		//sensorPos2 = sensorPos2 - dirW;
		Ray ray = make_Ray(sensorPos, dirW, RayTypeOpt::REINHART_RADIANCE, 0, RT_DEFAULT_MAX);
		//rtPrintf("%f %f %f\n", dirW.x, dirW.y, dirW.z);

		rtTrace(root, ray, pl);
		
		if (fmaxf(pl.value) > 0.0) {
			float3 value = pl.value * M_PIf/Ntot;
			//float3 value = pl.value/Ntot;
			
			atomicAdd(&coeff[pl.patchID], (float)value.x);
			//atomicAdd(&coeff[pl.patchID].x, (float)value.x);
		//	atomicAdd(&coeff[pl.patchID].y, (float)value.y);
			//atomicAdd(&coeff[pl.patchID].z, (float)value.z);
			//atomicAdd(&coeff[pl.patchID].z, (float)out);
		
			//ns[pl.patchID]++;
		}
		
		
	}

	rngs[pixelIdx] = pl.rng;

}


RT_PROGRAM void exception() {

	rtPrintExceptionDetails();

}


