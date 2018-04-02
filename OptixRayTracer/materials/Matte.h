#pragma once

#include "core/optix_global.h"
#include "brdfs/Lambertian.h"
#include "core/Ray.h"

class Matte {

public:

   RT_FUNCTION Matte();
   RT_FUNCTION Matte(Lambertian *brdf);
   RT_FUNCTION Lambertian* GetBRDF() const;
   RT_FUNCTION_HOST Material GetOptixMaterial(Context context);

private:

   Lambertian *brdf;

};

RT_FUNCTION Matte::Matte() { }

RT_FUNCTION Matte::Matte(Lambertian *brdf) 
   :brdf(brdf) {

}

RT_FUNCTION Lambertian* Matte::GetBRDF() const {

   return brdf;

}

RT_FUNCTION_HOST Material Matte::GetOptixMaterial(Context context) {

   //const char* path = "./Whitted.ptx";
   const char* path = "./PathTracer.ptx";
   Program closestHitRadiance = context->createProgramFromPTXFile(path, "closestHit");
   Program anyHit = context->createProgramFromPTXFile(path, "anyHit");
   Material optixMaterial = context->createMaterial();
   optixMaterial->setClosestHitProgram(RayType::RADIANCE, closestHitRadiance);
   optixMaterial->setAnyHitProgram(RayType::SHADOW, anyHit);
   optixMaterial["brdf"]->setUserData(sizeof(Lambertian), brdf);
   return optixMaterial;

}