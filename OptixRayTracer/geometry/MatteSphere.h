#pragma once

#include "shapes/Sphere.h"
#include "materials/Matte.h"
#include "core/optix_global.h"

class MatteSphere {

public:

	RT_FUNCTION MatteSphere();
	RT_FUNCTION MatteSphere(Sphere* sphere, Matte* material);
	RT_FUNCTION_HOST GeometryInstance GetOptixGeometry(Context context);

private:

	Sphere* sphere;
	Matte* material;
	GeometryInstance optixGeometryInstance;

};

RT_FUNCTION MatteSphere::MatteSphere() { }

RT_FUNCTION MatteSphere::MatteSphere(Sphere* sphere, Matte* material)
	: sphere(sphere), material(material), optixGeometryInstance(NULL) {


}

RT_FUNCTION_HOST GeometryInstance MatteSphere::GetOptixGeometry(Context context) {

	if (optixGeometryInstance == NULL) {
		Geometry geometry = sphere->GetOptixGeometry(context);
		Material  matte = material->GetOptixMaterial(context);
		optixGeometryInstance = context->createGeometryInstance(geometry, &matte, &matte + 1);
	}
	return optixGeometryInstance;

}