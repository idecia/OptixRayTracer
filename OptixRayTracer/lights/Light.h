#pragma once

#include "core/optix_global.h"

#define CALL_LIGHT_VIRTUAL_FUNCTION(lvalue, op, light, function, ...) \
    if (light->GetType() == Light::POINT ) \
        lvalue op (reinterpret_cast<PointLight*>(light))->function(__VA_ARGS__); \
		else if (light->GetType() == Light::AREA ) \
       lvalue op (reinterpret_cast<AreaLight*>(light))->function(__VA_ARGS__); 

class Light {

public:
      
   enum LightType {
     POINT,
     DIRECTIONAL,
     AREA,
     LIGHT_TYPE_COUNT
   };

   RT_FUNCTION Light() { };
   RT_FUNCTION Light(LightType type) : type(type) { }
   RT_FUNCTION LightType GetType() { return type; }

private:

   LightType type;

};
