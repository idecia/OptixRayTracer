#include "lights/PointLight.h"
#include "core/Ray.h"
#include "cameras/Pinhole"
#include "core/optix_global.h"


rtDeclareVariable(uint2, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(Pinhole, camera, , )
rtDeclareVariable(HitRecord, hit, , )
rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(Lambertian, brdf, , );
rtDeclareVariable(ShadowPayload, shadowPayload, rtPayload, );
rtDeclareVariable(RadiancePayload, radiancePayload, rtPayload, );


rtBuffer<PointLight>     lights;

RT_PROGRAM void closestHit() {

	//float3 color = make_float3(0.0f);
	float3 color = brdf.Eval(NULL) * 0.1; //simulate ambient light
	unsigned int numLights = lights.size();
	for (unsigned int i = 0; i < numLights; i++) {

		PointLight light = lights[i];
		float3 L = light.getPosition() - hit.position;
		float LDist = length(L);
		L = normalize(L);
		ShadowPayload shadowPayload;
		shadowPayload.attenuation = 1.0f;
		Ray shadowRay = make_Ray(hit.position, L, RayType::RADIANCE, shadowPayload, 0.05, LDist);
		rtTrace(root, shadowRay, shadowPaylod);
		if (shadowPayload.attenuation > 0.0f) {
			float nDotl = dot(hit.normal, L);
			float BRDF = brdf.Eval(NULL);
			color += BRDF * nDotl * light.getColor();
		}
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

