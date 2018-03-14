#include "lights/AreaLight.h"
#include "core/Ray.h"
#include "cameras/Pinhole.h"
#include "samplers\Random2D.h"
#include "samplers\Hemisphere.h"
#include "core/Payload.h"
#include "brdfs/Lambertian.h"
#include "core/optix_global.h"
#include <optix_device.h>

#define MAX_DEPTH 5

rtDeclareVariable(uint2, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(Pinhole, camera, , );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(Lambertian, brdf, , );
rtDeclareVariable(ShadowPayload, shadowPayload, rtPayload, );
rtDeclareVariable(RadiancePayload, radiancePayload, rtPayload, );
rtDeclareVariable(rtObject, root, , );
rtBuffer<AreaLight> lights;

RT_PROGRAM void closestHit() {

	//float3 color = make_float3(0.0f);
	BRDFQueryRecord  dummy;

	//ambient light
	float3 color = brdf.Eval(dummy) * 0.1; 

	//direct light
	unsigned int numLights = lights.size();
	int nSamples = 1;
	Random2D sampler(&radiancePayload.rng, nSamples);
	for (unsigned int i = 0; i < numLights; i++) {
		AreaLight light = lights[i];
		float2 uniformSample;
		while (sampler.Next2D(&uniformSample)) {
			float3 lightPoint = light.Sample(uniformSample);
			float3 L = lightPoint - hit.position;
			float LDist = length(L);
			L = normalize(L);
			ShadowPayload shadowPayload;
			shadowPayload.attenuation = 1.0f;
			Ray shadowRay = make_Ray(hit.position, L, RayType::SHADOW, 0.5, LDist);
			rtTrace(root, shadowRay, shadowPayload);
			if (shadowPayload.attenuation > 0.0f) {
				float nDotl  = fmaxf(dot(hit.normal, L), 0.0f);
				float lpDotl = fmaxf(dot(light.NormalAt(lightPoint), -L), 0.0f);
				float3 BRDF = brdf.Eval(dummy);
				float pdf = light.Pdf(lightPoint);
				color += BRDF * nDotl * lpDotl * light.GetColor() / (LDist * LDist * pdf);
			}
		}
		
	}
	color /= (float)nSamples;
	

	//indirect light
	if (radiancePayload.depth < MAX_DEPTH) {

		Hemisphere hsSampler(&radiancePayload.rng, 1);
		Onb onb(hit.normal);
		float3 direction;
		hsSampler.Next3D(&direction);
		onb.inverse_transform(direction);
		float nDotl = fmaxf(dot(hit.normal, direction), 0.0f);
		float3 BRDF = brdf.Eval(dummy);
		float pdf = brdf.Pdf(dummy);
		Ray radianceRay = make_Ray(hit.position, direction, RayType::RADIANCE, 0.5, RT_DEFAULT_MAX);
		RadiancePayload pl;
		pl.depth = radiancePayload.depth + 1;
		pl.rng = radiancePayload.rng;
		rtTrace(root, radianceRay, pl);
		color += BRDF * nDotl * pl.color / pdf;

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

