#pragma once

#include "core/optix_global.h"
#include "Light.h"
#include "shapes/TriangleMesh.h"

/*
class AreaLight : public Light {

public:

   RT_FUNCTION AreaLight();
   RT_FUNCTION AreaLight(const TriangleMesh &mesh, const float3 &color);
   RT_FUNCTION float3 Sample(const float2 &sample);
   RT_FUNCTION float3 Le(const float3& wo) const;
   RT_FUNCTION float Area() const;
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

RT_FUNCTION float AreaLight::Area() const {

	return mesh.Area();

}

RT_FUNCTION float3 AreaLight::Le(const float3& wo) const {

   return color;
}

RT_FUNCTION float AreaLight::Pdf(const float3 &point) const {

   return 1.0f / Area();

}

RT_FUNCTION float3 AreaLight::Sample(const float3 &point, RNG &rng,
   float3 &wo, float &pdf, float  &tMax)  {
   
   float3 meshPoint, normal;
   mesh.Sample(rng, meshPoint, normal);
   wo = meshPoint - point;
   tMax  = length(wo);
   float distanceSquared = tMax * tMax;
   wo = normalize(wo);
   pdf = distanceSquared / (dot(normal, -wo) * Area());
   float3 Le = dot(normal, -wo) > 0 ? color : make_float3(0.0f);
   return Le;

}
*/

class AreaLight {

public:

	RT_FUNCTION AreaLight();
	RT_FUNCTION AreaLight(const Parallelogram &parallelogram, const float3 &color);
	RT_FUNCTION float3 Sample(const float2 &sample);
	RT_FUNCTION float3 GetColor() const;
	RT_FUNCTION float Pdf(const float3 &point) const;
	RT_FUNCTION float3 NormalAt(const float3 &point) const;


private:

	Parallelogram parallelogram;
	float3 color;


};

RT_FUNCTION AreaLight::AreaLight() { }

RT_FUNCTION AreaLight::AreaLight(const Parallelogram &parallelogram, const float3 &color)
	:parallelogram(parallelogram), color(color) {

}

RT_FUNCTION float3 AreaLight::Sample(const float2 &sample) {

	return parallelogram.Sample(sample);
}

RT_FUNCTION float3 AreaLight::NormalAt(const float3 &point) const {

	return parallelogram.NormalAt(point);

}


RT_FUNCTION float3 AreaLight::GetColor() const {

	return color;
}

RT_FUNCTION float AreaLight::Pdf(const float3 &point) const {

	float area = parallelogram.GetArea();
	return 1.0f / area;

}