#pragma once

#include "core/optix_global.h"
#include "core/Ray.h"

using namespace optix;

struct BRDFQueryRecord {
   float3 point;
   float3 wi;
   float3 wo;
};

class Lambertian {

public:

   RT_FUNCTION Lambertian();
   RT_FUNCTION Lambertian(const float3 &color);
   RT_FUNCTION float Pdf(const BRDFQueryRecord &record) const;
   RT_FUNCTION float3 Sample(BRDFQueryRecord &record, const float2 &sample) const;
   RT_FUNCTION float3 Eval(const BRDFQueryRecord &record) const;

private:

   float3 color;

};

RT_FUNCTION Lambertian::Lambertian() { }

RT_FUNCTION Lambertian::Lambertian(const float3 &color)
   :color(color) {

}

RT_FUNCTION float Lambertian::Pdf(const BRDFQueryRecord &record) const {
   
   return 0.5 * M_1_PIf;
}

RT_FUNCTION float3 Lambertian::Sample(BRDFQueryRecord &record, const float2 &sample) const {

   //TODO: sample hemisphere using sample
   return M_1_PIf * color;  //<-- This is the correct term
   //return color;
}

RT_FUNCTION float3 Lambertian::Eval(const BRDFQueryRecord &record) const {

   return M_1_PIf * color; // <-- This is the correct term
   //return color;
}

