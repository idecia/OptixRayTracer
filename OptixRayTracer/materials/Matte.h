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
	Material optixMaterial;

};

RT_FUNCTION Matte::Matte() { }

RT_FUNCTION Matte::Matte(Lambertian *brdf) 
	:brdf(brdf), optixMaterial(NULL) {

}

RT_FUNCTION Lambertian* Matte::GetBRDF() const {

	return brdf;

}

RT_FUNCTION_HOST Material Matte::GetOptixMaterial(Context context) {

	if (optixMaterial == NULL) {
		const char* path = "./Whitted.ptx";
		Program closestHitRadiance = context->createProgramFromPTXFile(path, "closestHit");
		Program anyHit = context->createProgramFromPTXFile(path, "anyHit");
		optixMaterial = context->createMaterial();
		optixMaterial->setClosestHitProgram(RayType::RADIANCE, closestHitRadiance);
		optixMaterial->setAnyHitProgram(RayType::SHADOW, anyHit);
		optixMaterial["brdf"]->setUserData(sizeof(Lambertian), brdf);
	}
	return optixMaterial;

}