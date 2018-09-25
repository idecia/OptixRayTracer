#pragma once

#include "shapes/Triangle.h"
#include "materials/Matte.h"
#include "core/optix_global.h"

class MatteTriangle {

public:

	RT_FUNCTION MatteTriangle();
	RT_FUNCTION MatteTriangle(Triangle* triangle, Matte* material);
	RT_FUNCTION_HOST GeometryInstance GetOptixGeometry(Context context);

private:

	Triangle* triangle;
	Matte* material;

};

RT_FUNCTION MatteTriangle::MatteTriangle() { }

RT_FUNCTION MatteTriangle::MatteTriangle(Triangle* triangle, Matte* material)
	: triangle(triangle), material(material) {


}

RT_FUNCTION_HOST GeometryInstance MatteTriangle::GetOptixGeometry(Context context) {

	Geometry geometry = triangle->GetOptixGeometry(context);
	Material  matte = material->GetOptixMaterial(context);
	GeometryInstance optixGeometryInstance = context->createGeometryInstance(geometry, &matte, &matte + 1);
	return optixGeometryInstance;

}