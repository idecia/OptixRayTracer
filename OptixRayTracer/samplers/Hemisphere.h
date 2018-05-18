#pragma once

#include "core/RNG.h"
#include "core/optix_global.h"

class Hemisphere {

public:

   RT_FUNCTION Hemisphere();
   RT_FUNCTION Hemisphere(RNG *rng, uint32_t totalSamples);
   RT_FUNCTION bool Next3D(float3 *sample);


private:

   RNG* rng;
   uint32_t totalSamples;
   uint32_t generatedSamples;
};


RT_FUNCTION Hemisphere::Hemisphere() {

}


RT_FUNCTION Hemisphere::Hemisphere(RNG *rng, uint32_t totalSamples)
   : rng(rng), totalSamples(totalSamples), generatedSamples(0) {


}

RT_FUNCTION bool Hemisphere::Next3D(float3 *sample) {

   float u1 = rng->RandomFloat();
   float u2 = rng->RandomFloat();
   *sample  = UniformHemisphereSample(u1, u2);
   ++generatedSamples;
   return (generatedSamples <= totalSamples);

}


