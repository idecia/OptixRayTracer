
#include <optix_world.h>

#include "core/optix_global.h"
#include "core/Ray.h"
#include "core/Payload.h"
#include "samplers/Util.h"
#include "samplers/RNG.h"
#include "core/ONB.h"
#include "core/math.h"
#include "samplers/Random2D.h"
#include "skyes/Beckers288.h"
#include "lights/EnvironmentLight.h"
#include <optix_device.h>

rtBuffer<RNG> rngs;
//rtBuffer<float2> u;
rtBuffer<float> coeff;
//rtBuffer<float3> coeff;
//rtBuffer<double3> coeff;
rtDeclareVariable(int, Ntot, , );
rtDeclareVariable(int, nSamples, , );
rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(float3, sensorNormal, , );
rtDeclareVariable(float3, point, , );
rtDeclareVariable(float3, sensorPos, , );
rtDeclareVariable(rtObject, buildingWindows, , );
rtDeclareVariable(unsigned int, NskyPatches, , );
rtDeclareVariable(EnvironmentLight, light, , );


RT_PROGRAM void sensor(void) {


	BeckersPayload pl;
	pl.rng = rngs[pixelIdx];

	Random2D sampler(&pl.rng, nSamples);
	float2 unifSample; //= u[pixelIdx];

	while (sampler.Next2D(&unifSample)) {

		//pl.value = make_float3(0.0f);
		pl.value = make_float3(0.0f);
		pl.depth = 0;


		ONB onb(sensorNormal);
		float3 dir = CosineHemisphereSample(unifSample.x, unifSample.y);
		float3 dirW = onb.LocalToWorld(dir);
		//float3 sensorPos2 = make_float3(1.118916, 4.990000, 3.026103);
		//float3 sensorPos2 = make_float3(-1.444052f, -0.000000f, 3.0005f);
		//float3 sensorPos2 = make_float3(-1.490000f, -0.540149f, 0.997289f);
		//dirW = make_float3(0.945326f, 0.091734f, -0.312960f);
		//  dirW = make_float3( -0.456876, -0.88952, -0.004548);
		//sensorPos2 = sensorPos2 - dirW;
		Ray ray = make_Ray(sensorPos, dirW, RayTypeOpt::BECKERS_RADIANCE, 0, RT_DEFAULT_MAX);
		//rtPrintf("%f %f %f\n", dirW.x, dirW.y, dirW.z);

		rtTrace(buildingWindows, ray, pl);

		if (fmaxf(pl.value) > 0.0) {
			float3 value = pl.value * M_PIf / Ntot;
			//float3 value = pl.value;
			//rtPrintf("%d %f %f\n", pl.patchID, (float)value.x, (float)value.x);
			atomicAdd(&coeff[pl.dirID], (float)value.x);
			//atomicAdd(&coeff[pl.dirID].y, (float)value.y);
			//atomicAdd(&coeff[pl.dirID].z, (float)value.z);
			//ns[pl.patchID]++;
		}


	}

	rngs[pixelIdx] = pl.rng;

}


/*
RT_PROGRAM void sensor(void) {

	
	BeckersPayload pl;
	pl.rng = rngs[pixelIdx];

	Random2D sampler(&pl.rng, nSamples);
	float2 unifSample; //= u[pixelIdx];
	while (sampler.Next2D(&unifSample)) {

		
		pl.value = (M_PIf / Ntot) * make_float3(1.0f);
		pl.depth = 0;

		ONB onb(sensorNormal);
		float3 dir = CosineHemisphereSample(unifSample.x, unifSample.y);
		float3 dirW = onb.LocalToWorld(dir);

		Ray ray = make_Ray(sensorPos, dirW, RayTypeOpt::BECKERS_RADIANCE, 0, RT_DEFAULT_MAX);
		//rtPrintf("%f %f %f %f %f %f\n", sensorPos.x, sensorPos.y, sensorPos.z, dirW.x, dirW.y, dirW.z);
		rtTrace(buildingWindows, ray, pl);

	
	
	}

	rngs[pixelIdx] = pl.rng;

}*/

RT_PROGRAM void exception() {

	rtPrintExceptionDetails();

}

/*
RT_PROGRAM void sensor(void) {


	BeckersPayload pl;
	pl.rng = rngs[pixelIdx];

	Random2D sampler(&pl.rng, nSamples);
	float2 unifSample; //= u[pixelIdx];

	while (sampler.Next2D(&unifSample)) {


	
		pl.depth = 0;

		float3 wiW, L;
		float pdf, tMax;

		Random2D sampler(&pl.rng, 1);
		float2 uniformSample;
		sampler.Next2D(&uniformSample);
		L = light.Sample(sensorPos, uniformSample, wiW, pdf, tMax);
		ShadowPayload shadowPayload;
		shadowPayload.attenuation = 1.0f;
		Ray shadowRay = make_Ray(sensorPos, wiW, RayTypeOpt::BECKERS_SHADOW, 0.0, tMax-0.1);
		rtTrace(buildingWindows, shadowRay, shadowPayload);
		if (shadowPayload.attenuation > 0.0f) {
			ONB o(-light.parallelogram.NormalAt(make_float3(1.0f)));
			float3 v = o.WorldToLocal(wiW);
			int dirID = beckers(o.WorldToLocal(wiW));
			float cos = dot(wiW, sensorNormal) > 0 ? dot(wiW, sensorNormal) : 0.0f;
			float value = cos/ pdf;
			value = value / Ntot;
			atomicAdd(&coeff[dirID].x, (float)value);
			atomicAdd(&coeff[dirID].y, (float)value);
			atomicAdd(&coeff[dirID].z, (float)value);
		}

	}
	rngs[pixelIdx] = pl.rng;

}*/
