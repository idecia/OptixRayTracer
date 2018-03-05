#pragma once

#include "core/optix_global.h"
#include "brdfs/Lambertian"
#include "ray.h"

using namespace optix;

class Matte {

public:

	RT_FUNCTION Matte();
	RT_FUNCTION Matte(Lambertian *brdf);
	RT_FUNCTION Lambertian* GetBRDF() const;
	RT_FUNCTION_HOST Material getOptixMaterial(Context context);

private:

	Lambertian *brdf
	Material optixMaterial;

};

RT_FUNCTION Mate::Matte() { }

RT_FUNCTION Mate::Matte(Lambertian *brdf) 
	:brdf(brdf), optixMaterial(NULL) {

}

RT_FUNCTION Lambertian* Matte::GetBRDF() const {
	return brfd;
}

RT_FUNCTION_HOST Material Matte::getOptixMaterial(Context context) {

	if (optixMaterial == NULL) {
		String path = "./Whitted.ptx";
		Program closestHitRadiance = context->createProgramFromPTXString(path, "closestHit");
		Program anyHit = context->createProgramFromPTXString(path, "anyHit");
		optixMaterial = context->createMaterial();
		optixMaterial->setClosestHitProgram(RayType::RADIANCE, closestHitRadiance);
		optixMaterial->setAnyHitProgram(RayType::SHADOW, anyHit);
		optixMaterial["brdf"]->setUserData(sizeOf(Lambertian), brdf);

	}
	return optixMaterial;

}