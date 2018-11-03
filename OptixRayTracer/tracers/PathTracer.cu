#include "lights/AllLights.h"
#include "core/Ray.h"
#include "samplers/Random2D.h"
#include "core/Payload.h"
#include "bxdfs/Lambertian.h"
#include "core/optix_global.h"
#include "core/ONB.h"
#include <optix_device.h>

#define MAX_DEPTH 1

rtDeclareVariable(uint2, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(Lambertian, brdf, , );
rtDeclareVariable(ShadowPayload, shadowPayload, rtPayload, );
rtDeclareVariable(RadiancePayload, radiancePayload, rtPayload, );
rtDeclareVariable(rtObject, root, , );
rtBuffer<Light*> lights;



RT_FUNCTION float DirectLight(const ONB &onb, const float3 &woW, const float3 &wo) {

	float3 color = make_float3(0.0f);
	unsigned int nLights = lights.size();
	Random2D sampler(&radiancePayload.rng, nLights);
	for (unsigned int i = 0; i < nLights; i++) {
		Light* light = lights[i];
		float2 uniformSample;
		sampler.Next2D(&uniformSample);
		float3 wiW, L;
		float pdf, tMax;
		CALL_LIGHT_VIRTUAL_FUNCTION(Le, = , light, Sample, hit.position, uniformSample, wiW, pdf, tMax);
		ShadowPayload shadowPayload;
		shadowPayload.attenuation = 1.0f;
		Ray shadowRay = make_Ray(hit.position, wiW, RayType::SHADOW, 0.01, tMax);
		rtTrace(root, shadowRay, shadowPayload);
		if (shadowPayload.attenuation > 0.0f) {
			float3 wi = onb.WorldToLocal(wiW);
			float3 BRDF = brdf.Eval(wo, wi);
			float nDotWi = fmaxf(wi.z, 0.0f);
			color += BRDF * nDotWi * Le / pdf;
		}
	}
	color /= (float)nLights;
	return color;
}


RT_FUNCTION float IndirectLight(const ONB &onb, const float3 &woW, const float3 &wo) {

	Random2D sampler(&radiancePayload.rng, 1);
	float2 uniformSample;
	sampler.Next2D(&uniformSample);
	float3 wi;
	float pdf;
	float3 BRDF = brdf.Sample(wo, &wi, &pdf, uniformSample);
	float3 wiW = onb.LocalToWorld(wi);
	Ray radianceRay = make_Ray(hit.position, wiW, RayType::RADIANCE, 0, RT_DEFAULT_MAX);
	RadiancePayload newRadiancePayload;
	newRadiancePayload.depth = radiancePayload.depth + 1;
	newRadiancePayload.rng = radiancePayload.rng;
	rtTrace(root, radianceRay, newRadiancePayload);
	float nDotWi = fmaxf(wi.z, 0.0f);
	color += BRDF * nDotWi * newRadiancePayload.color / pdf;
	
}


RT_PROGRAM void closestHit() {

	ONB onb(hit.normal);
	float3 woW = -ray.direction;
	float3 wo = onb.WorldToLocal(woW);
	float3 color = make_float3(0.0f);
	color += DirectLight(onb, woW, wo);
	if (radiancePayload.depth < MAX_DEPTH) {
		color += IndirectLight(onb, woW, wo);
	}
	radiancePayload.color = color;
}

RT_PROGRAM void anyHit() {

   shadowPayload.attenuation = 0.0f;
   rtTerminateRay();

}

RT_PROGRAM void miss() {

   float3 BLACK = make_float3(0.0f);
   radiancePayload.color = BLACK;

}

