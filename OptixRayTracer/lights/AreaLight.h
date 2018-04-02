#pragma once

#include "core/optix_global.h"
#include "shapes/parallelogram.h"
#include "core/Light.h"

class AreaLight : public Light {

public:

   RT_FUNCTION AreaLight();
   RT_FUNCTION AreaLight(const Parallelogram &parallelogram, const float3 &color);
   RT_FUNCTION float3 Sample(const float2 &sample);
   RT_FUNCTION float3 GetColor() const;
   RT_FUNCTION float Pdf(const float3 &point) const;
   RT_FUNCTION float3 NormalAt(const float3 &point) const;
   RT_FUNCTION float3 Sample(const float3 &point, const float2 &sample, 
   float3 &wo, float &pdf, float  &tMax) ;

private:

   Parallelogram parallelogram;
   float3 color;


};

RT_FUNCTION AreaLight::AreaLight() { }

RT_FUNCTION AreaLight::AreaLight(const Parallelogram &parallelogram, const float3 &color)
   :Light(Light::AREA), parallelogram(parallelogram), color(color) {

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

RT_FUNCTION float3 AreaLight::Sample(const float3 &point, const float2 &sample,
   float3 &wo, float &pdf, float  &tMax)  {

   float3 ps = parallelogram.Sample(sample);
   float3 n  = parallelogram.NormalAt(ps);
   wo = ps - point;
   tMax  = length(wo);
   float distanceSquared = tMax * tMax;
   wo = normalize(wo);
   pdf = distanceSquared / (dot(n, -wo) * parallelogram.GetArea());
   float3 L = dot(n, -wo) > 0 ? color : make_float3(0.0f);
   return L;

}