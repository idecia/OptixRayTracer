#pragma once

#include "core/optix_global.h"

class Sphere {

public:

	RT_FUNCTION Sphere();
	RT_FUNCTION Sphere(const float3 &center, float radius);
	RT_FUNCTION float3 NormalAt(const float3 &point) const;
	RT_FUNCTION float3 GetCenter() const;
	RT_FUNCTION float GetRadius() const;
	RT_FUNCTION_HOST Geometry GetOptixGeometry(Context context);
	

private:

	float3 center;
	float radius;
	Geometry optixGeometry;

};


RT_FUNCTION Sphere::Sphere() { }

RT_FUNCTION Sphere::Sphere(const float3 &center, float radius)
	: center(center), radius(radius), optixGeometry(NULL) {

}

RT_FUNCTION float3 Sphere::NormalAt(const float3 &point) const {
	return (point - center) / radius;
}

RT_FUNCTION float3 Sphere::GetCenter() const {
	return center;
}

RT_FUNCTION float Sphere::GetRadius() const {
	return radius;
}

RT_FUNCTION_HOST Geometry Sphere::GetOptixGeometry(Context context) {

	if (optixGeometry == NULL) {
		optixGeometry = context->createGeometry();
		optixGeometry->setPrimitiveCount(1u);
		const char* path = "/Sphere.cu";
		optixGeometry->setBoundingBoxProgram(context->createProgramFromPTXFile(path, "boundingBox"));
		optixGeometry->setIntersectionProgram(context->createProgramFromPTXFile(path, "intersect"));
		optixGeometry["sphere"]->setUserData(sizeof(Sphere), this);
	}
	return optixGeometry;

}