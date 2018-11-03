#pragma once

#include "core/optix_global.h"
#include "Light.h"
#include "shapes/TriangleMesh.h"

class AreaLight : public Light {

public:

   RT_FUNCTION AreaLight();
   RT_FUNCTION AreaLight(const TriangleMesh &mesh, const float3 &color);
   RT_FUNCTION float3 Le(const float3& wo) const;
   RT_FUNCTION float GetArea() const;
   RT_FUNCTION float Pdf(const float3 &point) const;
   RT_FUNCTION float3 Sample(const float3 &point, RNG &rng, 
      float3 &wo, float &pdf, float  &tMax);

private:

   TriangleMesh mesh;
   float3 color;


};

RT_FUNCTION AreaLight::AreaLight() { }

RT_FUNCTION AreaLight::AreaLight(const TriangleMesh &mesh, const float3 &color)
   :Light(Light::AREA), mesh(mesh), color(color) {

}

RT_FUNCTION float AreaLight::GetArea() const {

	return mesh.GetArea();

}

RT_FUNCTION float3 AreaLight::Le(const float3& wo) const {

   return color;
}

RT_FUNCTION float AreaLight::Pdf(const float3 &point) const {

   return 1.0f / GetArea();

}

RT_FUNCTION float3 AreaLight::Sample(const float3 &point, RNG &rng,
   float3 &wo, float &pdf, float  &tMax)  {
   
   float3 meshPoint, meshNormal;
   mesh.Sample(rng, meshPoint, meshNormal);
   wo = meshPoint - point;
   tMax  = length(wo);
   float distanceSquared = tMax * tMax;
   wo = normalize(wo);
   pdf = distanceSquared / (dot(meshNormal, -wo) * GetArea());
   float3 Le = dot(meshNormal, -wo) > 0 ? color : make_float3(0.0f);
   return Le;

}
