#include "core/optix_global.h"
#include "core/RNG.H"

struct RadiancePayload {

   RNG rng;
   float3 color;
   int depth;

};

struct ShadowPayload {

   float attenuation;

   
};

