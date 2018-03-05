#pragma once

#include "shapes/Sphere.h"
#include "materials/Matte.h"

class MatteSphere {

public:

	RT_FUNCTION MatteSphere();
	RT_FUNCTION MatteSphere(Sphere* sphere, Matte* material);
	RT_FUNCTION_HOST GeometryInstance getOptixGeometry(Context context);

private:

	Sphere* sphere;
	Matte* material;
	GeometryInstance optixGeometryInstance;

};

RT_FUNCTION MatteSphere::MatteSphere() { }

RT_FUNCTION MatteSphere::MatteSphere(Sphere* sphere, Matte* material)
	: sphere(sphere), material(material), optixGeometryInstance(NULL) {


}

RT_FUNCTION_HOST GeometryInstance MatteSphere::getOptixGeometry(Context context) {

	if (optixGeometryInstance == NULL) {
		Geometry sphere = sphere->getOptixGeometry(context);
		Material  matte = material->getOptixMaterial(context);
		optixGeometryInstance = context->createGeometryInstance(sphere, &matte, &matte + 1)
	}
	return optixGeometryInstance;

}