#pragma once

#include <stdint.h>
#include "core/optix_global.h"
#include <curand_kernel.h>

class RNG {

public:

   RT_FUNCTION RNG();
   RT_FUNCTION RNG(uint32_t seed, uint32_t index);
   RT_FUNCTION float RandomFloat();
   RT_FUNCTION uint32_t RandomUInt();
   RT_FUNCTION uint32_t RandomUInt(uint32_t bound);

private:

   curandState state;

};

RT_FUNCTION uint32_t WangHash(uint32_t seed) {

   seed = (seed ^ 61) ^ (seed >> 16);
   seed *= 9;
   seed = seed ^ (seed >> 4);
   seed *= 0x27d4eb2d;
   seed = seed ^ (seed >> 15);
   return seed;
}

RT_FUNCTION RNG::RNG() { }

RT_FUNCTION RNG::RNG(uint32_t seed, uint32_t index) {

   curand_init(WangHash(seed + index), 0, 0, &state);
}

RT_FUNCTION float RNG::RandomFloat() {

   return curand_uniform(&state);
}

RT_FUNCTION uint32_t RNG::RandomUInt() {

   // Here we assume that curand returns a 32-bit
   // unsigned int so we can cast it to a uint32_t.
   return (uint32_t)curand(&state);
}

RT_FUNCTION uint32_t RNG::RandomUInt(uint32_t bound) {

   // Extracted from the PCG random number generator developed by
   // Melissa O'Neill. See http://www.pcg-random.org

   // To avoid bias, we need to make the range of the RNG a multiple of
   // bound, which we do by dropping output less than a threshold.
   // A naive scheme to calculate the threshold would be to do
   //
   //     uint32_t threshold = 0x100000000ull % bound;
   //
   // but 64-bit div/mod is slower than 32-bit div/mod (especially on
   // 32-bit platforms).  In essence, we do
   //
   //     uint32_t threshold = (0x100000000ull-bound) % bound;
   //
   // because this version will calculate the same modulus, but the LHS
   // value is less than 2^32.

   uint32_t threshold = (~bound + 1u) % bound;

   // Uniformity guarantees that this loop will terminate.  In practice, it
   // should usually terminate quickly; on average (assuming all bounds are
   // equally likely), 82.25% of the time, we can expect it to require just
   // one iteration.  In the worst case, someone passes a bound of 2^31 + 1
   // (i.e., 2147483649), which invalidates almost 50% of the range.  In
   // practice, bounds are typically small and only a tiny amount of the range
   // is eliminated.
   for (;;) {
      uint32_t r = RandomUInt();
      if (r >= threshold)
         return r % bound;
   }

}