#include "lights/AllLights.h"
#include "core/Ray.h"
#include "cameras/Pinhole.h"
#include "samplers\Random2D.h"
#include "samplers\Hemisphere.h"
#include "core/Payload.h"
#include "brdfs/Lambertian.h"
#include "core/optix_global.h"
#include <optix_device.h>

#define MAX_DEPTH 0

rtDeclareVariable(uint2, pixelIdx, rtLaunchIndex, );
rtDeclareVariable(Pinhole, camera, , );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );
rtDeclareVariable(Lambertian, brdf, , );
rtDeclareVariable(ShadowPayload, shadowPayload, rtPayload, );
rtDeclareVariable(RadiancePayload, radiancePayload, rtPayload, );
rtDeclareVariable(rtObject, root, , );
rtBuffer<Light*> lights;

RT_PROGRAM void closestHit() {

   float3 color = make_float3(0.0f);
   BRDFQueryRecord  dummy;

   //ambient light
   //float3 color = brdf.Eval(dummy) * 0.1; 

   //direct light
   unsigned int numLights = lights.size();
   int nSamples = numLights;
   Random2D sampler(&radiancePayload.rng, nSamples);
   for (unsigned int i = 0; i < numLights; i++) {
      Light* light = lights[i];
      float2 uniformSample;
      //while (sampler.Next2D(&uniformSample)) {
	  sampler.Next2D(&uniformSample);
         float3 wo, L; 
         float pdf, tMax;
		 CALL_LIGHT_VIRTUAL_FUNCTION(L, = , light, Sample, hit.position, uniformSample, wo, pdf, tMax);
       //  L = light->Sample(hit.position, uniformSample,  wo, pdf, tMax);
         /*float3 lightPoint = light.Sample(uniformSample);
         float3 L = lightPoint - hit.position;
         float LDist = length(L);
         L = normalize(L);*/
         ShadowPayload shadowPayload;
         shadowPayload.attenuation = 1.0f;
         Ray shadowRay = make_Ray(hit.position, wo, RayType::SHADOW, 0.1, tMax);
         rtTrace(root, shadowRay, shadowPayload);
         if (shadowPayload.attenuation > 0.0f) {
            /*float nDotl  = fmaxf(dot(hit.normal, L), 0.0f);
            float lpDotl = fmaxf(dot(light.NormalAt(lightPoint), -L), 0.0f);
            float3 BRDF = brdf.Eval(dummy);
            float pdf = light.Pdf(lightPoint);
            color += BRDF * nDotl * lpDotl * light.GetColor() / (LDist * LDist * pdf);*/
            float3 BRDF = brdf.Eval(dummy);
            float nDotl = fmaxf(dot(hit.normal, wo), 0.0f);
            color += BRDF * nDotl * L  / pdf;
         }
     // }
      
   }
   color /= (float)nSamples;
   
   
   //indirect light
   if (radiancePayload.depth < MAX_DEPTH) {

      Hemisphere hsSampler(&radiancePayload.rng, 1);
      Onb onb(hit.normal);
      float3 direction;
      hsSampler.Next3D(&direction);
      //cosine_sample_hemisphere(radiancePayload.rng.RandomFloat(), radiancePayload.rng.RandomFloat(), direction);
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
      //color += BRDF * pl.color;

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

