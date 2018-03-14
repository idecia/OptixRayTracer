#include "lights/PointLight.h"
#include "core/Ray.h"
#include "cameras/Pinhole.h"
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
rtBuffer<PointLight> lights;

RT_PROGRAM void closestHit() {

	//float3 color = make_float3(0.0f);
	BRDFQueryRecord  dummy;
	float3 color = brdf.Eval(dummy) * 0.1; //simulate ambient light}
	unsigned int numLights = lights.size();
	for (unsigned int i = 0; i < numLights; i++) {

		PointLight light = lights[i];
		float3 L = light.GetPosition() - hit.position;
		float LDist = length(L);
		L = normalize(L);
		ShadowPayload shadowPayload;
		shadowPayload.attenuation = 1.0f;
		Ray shadowRay = make_Ray(hit.position, L, RayType::SHADOW, 0.1, LDist);
		rtTrace(root, shadowRay, shadowPayload);
		if (shadowPayload.attenuation > 0.0f) {
			float nDotl = dot(hit.normal, L);
			float3 BRDF = brdf.Eval(dummy);
			color += BRDF * nDotl * light.GetColor();
		}
	}
	//color.x = 1; color.y = color.z = 0;
	radiancePayload.color = color;
	//rtPrintf("(%f, %f, %f)\n", hit.position.x, hit.position.y, hit.position.z);
	
}

RT_PROGRAM void anyHit() {

	shadowPayload.attenuation = 0.0f;
	rtTerminateRay();

}

RT_PROGRAM void miss() {

	float3 BLACK = make_float3(0.0f);
	radiancePayload.color = BLACK;

}

