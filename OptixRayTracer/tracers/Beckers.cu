#include "skyes/Beckers288.h"
#include "core/Ray.h"
#include "samplers/Random2D.h"
#include "core/Payload.h"
#include "bxdfs/Lambertian.h"
#include "lights/EnvironmentLight.h"
#include "core/optix_global.h"
#include "core/ONB.h"
#include "core/math.h"
#include <optix_device.h>

#define MAX_DEPTH 4


rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(Lambertian, brdf, , );
rtDeclareVariable(float3, point, , );
rtDeclareVariable(float3, sensorNormal, , );
rtDeclareVariable(BeckersPayload, beckersPayload, rtPayload, );
rtDeclareVariable(ShadowPayload, shadowPayload, rtPayload, );
rtDeclareVariable(rtObject, buildingWindows, , );
rtDeclareVariable(EnvironmentLight, light, , );
rtBuffer<float3> coeff;
rtDeclareVariable(float3, Le, , );

/*
RT_PROGRAM void closestHit() {

double3 value = make_double3(0.0, 0.0, 0.0);
ONB onb(hit.normal);
float3 woW = -ray.direction;
float3 wo = onb.WorldToLocal(woW);

if (BeckersPayload.depth < MAX_DEPTH) {
Random2D sampler(&BeckersPayload.rng, 1);
float2 uniformSample;
sampler.Next2D(&uniformSample);
float3 wi;
float pdf;
//float3 BRDF = brdf.Sample(wo, &wi, &pdf, uniformSample);
float3 wiW = onb.LocalToWorld(wi);
double3 BRDF = ToDouble(brdf.Sample(wo, &wi, &pdf, uniformSample));
double3 wiWd = ToDouble(wiW);
Ray radianceRay = make_Ray(hit.position + 0.0001*hit.normal, wiW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
BeckersPayload newBeckersPayload;
newBeckersPayload.depth = BeckersPayload.depth + 1;
newBeckersPayload.rng = BeckersPayload.rng;
rtTrace(root, radianceRay, newBeckersPayload);
//float nDotWi = fabsf(wi.z);
//float nDotWi = fabsf(dot(wiW,hit.normal));
double3 normal = ToDouble(hit.normal);
double nDotWi = fabs(dot(wiWd, normal));
//value = BRDF * nDotWi * newBeckersPayload.value / pdf;
value = BRDF * nDotWi * newBeckersPayload.value / (double)pdf;
BeckersPayload.patchID = newBeckersPayload.patchID;

//rtPrintf("%f %f %f\n", BRDF.x, BRDF.y, BRDF.z);
}
BeckersPayload.value = value;
}
*/


/*RT_PROGRAM void closestHit2() {

	ONB onb(hit.normal);
	ONB onbSensor(sensorNormal);
	float3 woW = -ray.direction;
	float3 wo = onb.WorldToLocal(woW);

	if ((beckersPayload.depth == 0) && (Le.x > 0)) {
		float3 value = beckersPayload.value;
		float3 dirID = beckers(onbSensor.WorldToLocal(-woW));
		atomicAdd(&coeff[dirID].x, (float)value.x);
		atomicAdd(&coeff[dirID].y, (float)value.y);
		atomicAdd(&coeff[dirID].z, (float)value.z);
		return;
	}


	//direct light
	unsigned int nLights = 1;
	int nSamples = 1;
	Random2D sampler(&beckersPayload.rng, nSamples);
	for (unsigned int i = 0; i < nLights; i++) {
		float2 uniformSample;
		sampler.Next2D(&uniformSample);
		float3 wiW, L;
		float pdf, tMax;
		L = light.Sample(hit.position, uniformSample, wiW, pdf, tMax);
		ShadowPayload shadowPayload;
		shadowPayload.attenuation = 1.0f;
		Ray shadowRay = make_Ray(hit.position, wiW, RayType::BECKERS_SHADOW, 0.01, tMax);
		rtTrace(buildingWindows, shadowRay, shadowPayload);
		if (shadowPayload.attenuation > 0.0f) {
			float3 wi = onb.WorldToLocal(wiW);
			float3 BRDF = brdf.Eval(wo, wi);
			float nDotWi = fmaxf(wi.z, 0.0f);
			float3 value = beckersPayload.value * BRDF * nDotWi * L / pdf;
			flaot3 dirID = beckers(onbSensor.WorldToLocal(wiW));
			atomicAdd(&coeff[dirID].x, (float)value.x);
			atomicAdd(&coeff[dirID].y, (float)value.y);
			atomicAdd(&coeff[dirID].z, (float)value.z);
		}
	}

	if (beckersPayload.depth < MAX_DEPTH) {
		Random2D sampler(&beckersPayload.rng, 1);
		float2 uniformSample;
		sampler.Next2D(&uniformSample);
		float3 wi;
		float pdf;
		float3 BRDF;

		BRDF = brdf.Sample(wo, &wi, &pdf, uniformSample);
		//	rtPrintf("%f \n", brdf.getR().x);

		float3 wiW = onb.LocalToWorld(wi);
		Ray radianceRay;
		radianceRay = make_Ray(hit.position + 0.00*hit.normal, wiW, RayType::BECKERS_RADIANCE, 0, RT_DEFAULT_MAX);

		BeckersPayload newBeckersPayload;
		newBeckersPayload.depth = beckersPayload.depth + 1;
		newBeckersPayload.rng = beckersPayload.rng;
		newBeckersPayload.value = beckersPayload.value * BRDF * nDotWi * pdf;
		rtTrace(buildingWindows, radianceRay, newBeckersPayload);
	}
}
*/

RT_PROGRAM void closestHit() {

	ONB onb(hit.normal);
	ONB onbSensor(sensorNormal);
	float3 woW = -ray.direction;
	float3 wo = onb.WorldToLocal(woW);
	if (Le.x > 0) {
		beckersPayload.value = make_float3(1.0);
		beckersPayload.dirID = beckers(onbSensor.WorldToLocal(-woW));
		return;
	}


	float3 value = make_float3(0.0f);
	if (beckersPayload.depth < MAX_DEPTH) {
		Random2D sampler(&beckersPayload.rng, 1);
		float2 uniformSample;
		sampler.Next2D(&uniformSample);
		float3 wi;
		float pdf;
		float3 BRDF;

		BRDF = brdf.Sample(wo, &wi, &pdf, uniformSample);
		//	rtPrintf("%f \n", brdf.getR().x);

		float3 wiW = onb.LocalToWorld(wi);
		Ray radianceRay;
		radianceRay = make_Ray(hit.position + 0.00*hit.normal, wiW, RayTypeOpt::BECKERS_RADIANCE, 0, RT_DEFAULT_MAX);

		BeckersPayload newBeckersPayload;
		newBeckersPayload.depth = beckersPayload.depth + 1;
		newBeckersPayload.rng = beckersPayload.rng;
		rtTrace(buildingWindows, radianceRay, newBeckersPayload);
		//float nDotWi = fabsf(wi.z);
		float nDotWi = fabsf(dot(wiW, hit.normal));
		value = BRDF * nDotWi * newBeckersPayload.value / pdf;
		beckersPayload.dirID = newBeckersPayload.dirID;

		//rtPrintf("%f %f %f\n", BRDF.x, BRDF.y, BRDF.z);
	}
	beckersPayload.value = value;
}



RT_PROGRAM void anyHit() {

	shadowPayload.attenuation = 0.0f;
	rtTerminateRay();

}

RT_PROGRAM void miss() {

	//BeckersPayload.value = make_double3(1.0,1.0,1.0);

	beckersPayload.value = make_float3(0.0);
	beckersPayload.dirID = 0;
	//if (BeckersPayload.patchID == 0) {
	//float t = -ray.origin.y / ray.direction.y;
	//rtPrintf(" %f %f %f %f %f %f \n", (ray.origin + t* ray.direction).x, (ray.origin + t* ray.direction).y, (ray.origin + t* ray.direction).z, ray.direction.x, ray.direction.y, ray.direction.z);

	//rtPrintf("%d %f %f %f %f %f %f \n", BeckersPayload.depth, ray.origin.x, ray.origin.y, ray.origin.z, ray.direction.x, ray.direction.y, ray.direction.z);

	//rtPrintf(" %f %f %f    %f %f %f\n", ray.origin.x, ray.origin.y, ray.origin.z, (ray.origin + 10 * ray.direction).x, (ray.origin + 10 * ray.direction).y, (ray.origin + 10 * ray.direction).z);

	//	rtPrintf("(%f %f %f)  (%f %f %f) %d %f \n   ", ray.origin.x, ray.origin.y, ray.origin.z, ray.direction.x, ray.direction.y, ray.direction.z, BeckersPayload.patchID, BeckersPayload.value.x);
	//rtPrintf("%d\n",  BeckersPayload.patchID);

	//rtPrintf("%f %f %f   \n   ", ray.origin.x, ray.origin.y, ray.origin.z);*/ 
	//}
}

