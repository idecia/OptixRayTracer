#pragma once

#include "RNG.h"
#include "core/optix_global.h"

class Random2D {

public:

   RT_FUNCTION Random2D();
   RT_FUNCTION Random2D(RNG *rng, uint32_t totalSamples);
   RT_FUNCTION bool Next2D(float2 *sample); 


private:

   RNG* rng;
   uint32_t totalSamples;
   uint32_t generatedSamples;
};


RT_FUNCTION Random2D::Random2D() {

}


RT_FUNCTION Random2D::Random2D(RNG *rng, uint32_t totalSamples) 
   : rng(rng), totalSamples(totalSamples), generatedSamples(0) {
   

}

RT_FUNCTION bool Random2D::Next2D(float2 *sample) {

   sample->x = rng->RandomFloat();
   sample->y = rng->RandomFloat();
   ++generatedSamples;
  return (generatedSamples <= totalSamples);

}


