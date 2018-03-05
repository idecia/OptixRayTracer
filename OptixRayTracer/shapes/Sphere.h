#pragma once

#include "core/optix_global.h"

class Sphere {

public:

	RT_FUNCTION Sphere();
	RT_FUNCTION Sphere(const float3 &center, float radius);
	RT_FUNCTION float3 NormalAt(const float3 &point);
	RT_FUNCTION float3 GetCenter() const;
	RT_FUNCTION float GetRadius() const;
	RT_FUNCTION_HOST Geometry getOptixGeometry(Context context);
	

private:
	float3 center;
	float radius;
	Geometry optixGeometry;

};


RT_FUNCTION Sphere::Sphere() { }

RT_FUNCTION Sphere::Sphere(const float3 &center, float radius)
	: center(center), radius(radius), optixGeometry(NULL) {

}

RT_FUNCTION float3 Sphere::NormalAt(const float3 &point) {
	return (point - center) / radius;
}

RT_FUNCTION float3 Sphere::GetCenter() const {
	return center;
}

RT_FUNCTION float Sphere::GetRadius() const {
	return radius;
}

RT_FUNCTION_HOST Geometry Sphere::getOptixGeometry(Context context) {

	if (geometry == NULL) {
		geometry = context->createGeometry();
		geometry->setPrimitiveCount(1u);
		const char* path = "/Sphere.cu";
		geometry->setBoundingBoxProgram(context->createProgramFromPTXString(path, "boundingBox"));
		geometry->setIntersectionProgram(context->createProgramFromPTXString(path, "intersect"));
		geometry["sphere"]->setUserData(sizeOf(Sphere), this);
	}
	return geometry;

}