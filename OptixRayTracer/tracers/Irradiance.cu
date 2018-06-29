#include "skyes/Reinhart145.h"
#include "core/Ray.h"
#include "samplers/Random2D.h"
#include "samplers/Hemisphere.h"
#include "core/Payload.h"
#include "bxdfs/Lambertian.h"
#include "bxdfs/ThinGlass.h"
#include "core/optix_global.h"
#include "core/ONB.h"
#include "core/math.h"
#include <optix_device.h>

#define MAX_DEPTH 6

rtDeclareVariable(uint, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(Lambertian, brdf, , );
rtDeclareVariable(uint, glass, , );
rtDeclareVariable(ThinGlass, brdfGlass, , );
rtDeclareVariable(float3, point, , );
rtDeclareVariable(ReinhartPayload, reinhartPayload, rtPayload, );
rtDeclareVariable(ShadowPayload, shadowPayload, rtPayload, );
rtDeclareVariable(rtObject, root, , );
/*
RT_PROGRAM void closestHit() {
	
	double3 value = make_double3(0.0, 0.0, 0.0);
	ONB onb(hit.normal);
	float3 woW = -ray.direction;
	float3 wo = onb.WorldToLocal(woW);

	if (reinhartPayload.depth < MAX_DEPTH) {
		Random2D sampler(&reinhartPayload.rng, 1);
		float2 uniformSample;
		sampler.Next2D(&uniformSample);
		float3 wi;
		float pdf;
		//float3 BRDF = brdf.Sample(wo, &wi, &pdf, uniformSample);
		float3 wiW = onb.LocalToWorld(wi);
		double3 BRDF = ToDouble(brdf.Sample(wo, &wi, &pdf, uniformSample));
		double3 wiWd = ToDouble(wiW);
		Ray radianceRay = make_Ray(hit.position + 0.0001*hit.normal, wiW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
		ReinhartPayload newReinhartPayload;
		newReinhartPayload.depth = reinhartPayload.depth + 1;
		newReinhartPayload.rng = reinhartPayload.rng;
		rtTrace(root, radianceRay, newReinhartPayload);
		//float nDotWi = fabsf(wi.z);
		//float nDotWi = fabsf(dot(wiW,hit.normal));
		double3 normal = ToDouble(hit.normal);
		double nDotWi = fabs(dot(wiWd, normal));
		//value = BRDF * nDotWi * newReinhartPayload.value / pdf;
		value = BRDF * nDotWi * newReinhartPayload.value / (double)pdf;
		reinhartPayload.patchID = newReinhartPayload.patchID;
		
			//rtPrintf("%f %f %f\n", BRDF.x, BRDF.y, BRDF.z);
	}
	reinhartPayload.value = value;
}
*/

RT_PROGRAM void closestHit() {
	
	//	rtPrintf("Hit: %f %f %f %\n", hit.position.x, hit.position.y, hit.position.z);
	//	return;
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
		float3 BRDF;
		if ((glass == 0)) {
			BRDF = brdf.Sample(wo, &wi, &pdf, uniformSample);
		//	rtPrintf("%f \n", brdf.getR().x);
		}
		else {
			BRDF = brdfGlass.Sample(wo, &wi, &pdf, uniformSample);
			//float cost = brdfGlass.F.SnellCosine(AbsCosTheta(wo));
		//	float r= brdfGlass.F.Reflectivity(CosTheta(wo), cost);
			//rtPrintf("%f %f %f %f %f %f\n", wi.x, wi.y, wi.z, wo.x,wo.y,wo.z);
		}
		float3 wiW = onb.LocalToWorld(wi);
		Ray radianceRay;
		if (glass==0)
			 radianceRay = make_Ray(hit.position + 0.00*hit.normal, wiW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
		else {
			//rtPrintf("%f %f %f %f %f %f \n", hit.position.x,hit.position.y,hit.position.z, wiW.x,wiW.y,wiW.z);
			float3 hitP = hit.position;
			hitP.y += 0.01*wiW.y;

			radianceRay = make_Ray(hitP, wiW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
		}
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

	//reinhartPayload.value = make_double3(1.0,1.0,1.0);
	reinhartPayload.value = make_float3(1.0f);
	reinhartPayload.patchID = reinhart(ray.direction, 1);

	if (reinhartPayload.patchID == 0) {
		//float t = -ray.origin.y / ray.direction.y;
		//rtPrintf(" %f %f %f %f %f %f \n", (ray.origin + t* ray.direction).x, (ray.origin + t* ray.direction).y, (ray.origin + t* ray.direction).z, ray.direction.x, ray.direction.y, ray.direction.z);

		//rtPrintf("%d %f %f %f %f %f %f \n", reinhartPayload.depth, ray.origin.x, ray.origin.y, ray.origin.z, ray.direction.x, ray.direction.y, ray.direction.z);

		//rtPrintf(" %f %f %f    %f %f %f\n", ray.origin.x, ray.origin.y, ray.origin.z, (ray.origin + 10 * ray.direction).x, (ray.origin + 10 * ray.direction).y, (ray.origin + 10 * ray.direction).z);

	//	rtPrintf("(%f %f %f)  (%f %f %f) %d %f \n   ", ray.origin.x, ray.origin.y, ray.origin.z, ray.direction.x, ray.direction.y, ray.direction.z, reinhartPayload.patchID, reinhartPayload.value.x);
		//rtPrintf("%d\n",  reinhartPayload.patchID);

		//rtPrintf("%f %f %f   \n   ", ray.origin.x, ray.origin.y, ray.origin.z);*/ 
	}
}

