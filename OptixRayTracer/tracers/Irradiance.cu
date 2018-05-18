#include "lights/Reinhart.h"
#include "core/Ray.h"
#include "samplers/Random2D.h"
#include "samplers/Hemisphere.h"
#include "core/Payload.h"
#include "brdfs/Lambertian.h"
#include "core/optix_global.h"
#include "core/ONB.h"
#include <optix_device.h>

#define MAX_DEPTH 20

rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(Lambertian, brdf, , );
rtDeclareVariable(ReinhartPayload, reinhartPayload, rtPayload, );
rtDeclareVariable(ShadowPayload, shadowPayload, rtPayload, );
rtDeclareVariable(rtObject, root, , );

RT_PROGRAM void closestHit() {
	
	float3 value = make_float3(0.0f);
	ONB onb(hit.normal);
	float3 woW = -ray.direction;
	float3 wo = onb.WorldToLocal(woW);

	if (reinhartPayload.depth < MAX_DEPTH) {
		Random2D sampler(&reinhartPayload.rng, 1);
		float2 uniformSample;
		sampler.Next2D(&uniformSample);
		float3 wi;
		float pdf;
		float3 BRDF = brdf.Sample(wo, &wi, &pdf, uniformSample);
		float3 wiW = onb.LocalToWorld(wi);
		Ray radianceRay = make_Ray(hit.position + 0.0001*hit.normal, wiW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
		ReinhartPayload newReinhartPayload;
		newReinhartPayload.depth = reinhartPayload.depth + 1;
		newReinhartPayload.rng = reinhartPayload.rng;
		rtTrace(root, radianceRay, newReinhartPayload);
		//float nDotWi = fabsf(wi.z);
		float nDotWi = fabsf(dot(wiW,hit.normal));
		value = BRDF * nDotWi * newReinhartPayload.value / pdf;
		reinhartPayload.patchID = newReinhartPayload.patchID;
		
			//rtPrintf("%f %f %f\n", BRDF.x, BRDF.y, BRDF.z);
	}
	reinhartPayload.value = value;
}

RT_PROGRAM void anyHit() {

	shadowPayload.attenuation = 0.0f;
	rtTerminateRay();

}

RT_PROGRAM void miss() {

	reinhartPayload.value = make_float3(1.0f);
	reinhartPayload.patchID = reinhart(ray.direction, 1);

//	if (reinhartPayload.patchID == 139)
		//rtPrintf("(%f %f %f)  (%f %f %f) %d %f \n   ", ray.origin.x, ray.origin.y, ray.origin.z, ray.direction.x, ray.direction.y, ray.direction.z, reinhartPayload.patchID, reinhartPayload.value.x);
		
		//rtPrintf("%f %f %f   \n   ", ray.origin.x, ray.origin.y, ray.origin.z);
}
