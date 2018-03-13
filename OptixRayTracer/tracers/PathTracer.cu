#include "lights/AreaLight.h"
#include "core/Ray.h"
#include "cameras/Pinhole.h"
#include "samplers\Random2D.h"
#include "core/Payload.h"
#include "brdfs/Lambertian.h"
#include "core/optix_global.h"
#include <optix_device.h>


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
			float3 L = light.sample(uniformSample) - hit.position;
			float LDist = length(L);
			L = normalize(L);
			ShadowPayload shadowPayload;
			shadowPayload.attenuation = 1.0f;
			Ray shadowRay = make_Ray(hit.position, L, RayType::SHADOW, 0.6, LDist);
			rtTrace(root, shadowRay, shadowPayload);
			if (shadowPayload.attenuation > 0.0f) {
				float nDotl = dot(hit.normal, L);
				float3 BRDF = brdf.Eval(dummy);
				color += BRDF * nDotl * light.GetColor();
			}
		}
		
	}
	color /= (float)nSamples;
	
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

