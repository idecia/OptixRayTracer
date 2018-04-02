#pragma once

#include "core/optix_global.h"
#include "core/Light.h"


class PointLight : public Light {

public:

   RT_FUNCTION PointLight();
   RT_FUNCTION PointLight(const float3 &position, float3 color); 
   RT_FUNCTION float3 GetPosition() const;
   RT_FUNCTION float3 GetColor() const;
   RT_FUNCTION float3 Sample(const float3 &point, const float2 &sample,
      float3 &wo, float &pdf, float &tMax) ;

   
private:

   float3 position;
   float3 color;
   

};

RT_FUNCTION PointLight::PointLight() { }

RT_FUNCTION PointLight::PointLight(const float3 &position, float3 color)
   :Light(Light::POINT), position(position), color(color) {

}

RT_FUNCTION float3 PointLight::GetPosition() const {

   return position;
}

RT_FUNCTION float3 PointLight::GetColor() const {

   return color;
}

RT_FUNCTION float3 PointLight::Sample(const float3 &point, const float2 &sample,
   float3 &wo, float &pdf, float  &tMax) {

   wo = position - point;
   tMax = length(wo);
   float distanceSquared = tMax*tMax;
   wo = normalize(wo);
   pdf = 1.0f;
   return color / distanceSquared;

}