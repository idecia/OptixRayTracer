#pragma once

#include "core/optix_global.h"

#include <optix_world.h>
#include <optixu/optixpp_namespace.h>
#include "core/Ray.h"
using namespace optix;

class Parallelogram{

public:

	RT_FUNCTION Parallelogram();
	RT_FUNCTION Parallelogram(const float3 &p0, const float3 &p1, const float3 &p2, bool ccw);
	RT_FUNCTION float3 NormalAt(const float3 &point) const;
	RT_FUNCTION float3 GetP0() const;
	RT_FUNCTION float3 GetP1() const;
	RT_FUNCTION float3 GetP2() const;
	RT_FUNCTION float4 GetPlane() const;
	RT_FUNCTION float GetArea() const;
	RT_FUNCTION float3 Sample(const float2 &sample);
	RT_FUNCTION_HOST Geometry GetOptixGeometry(Context context);


private:

	float3 p0, p1, p2;
	float4 plane;  

};


RT_FUNCTION Parallelogram::Parallelogram() { }

RT_FUNCTION Parallelogram::Parallelogram(const float3 &p0, const float3 &p1, const float3 &p2, bool ccw)
	: p0(p0), p1(p1), p2(p2) {

	int orientation = ccw ? 1 : -1;
	float3 normal = orientation * normalize(cross(p1 - p0, p2 - p0));
	float d       = dot(normal, p0);
	plane         = make_float4(normal, d);
}

RT_FUNCTION float3 Parallelogram::NormalAt(const float3 &point) const {

	return  make_float3(plane);

}

RT_FUNCTION float3 Parallelogram::GetP0() const {

	return p0;
}

RT_FUNCTION float3 Parallelogram::GetP1() const {

	return p1;
}

RT_FUNCTION float3 Parallelogram::GetP2() const {

	return p2;
}

RT_FUNCTION float4 Parallelogram::GetPlane() const {

	return plane;
}

RT_FUNCTION float3 Parallelogram::Sample(const float2 &sample) {

	float3 side1 = p1 - p0;
	float3 side2 = p2 - p0;
	return p0 + side1 * sample.x + side2 * sample.y;

}


RT_FUNCTION_HOST Geometry Parallelogram::GetOptixGeometry(Context context) {

	Geometry optixGeometry = context->createGeometry();
	optixGeometry->setPrimitiveCount(1u);
	const char* path = "./Parallelogram.ptx";
	optixGeometry->setBoundingBoxProgram(context->createProgramFromPTXFile(path, "boundingBox"));
	optixGeometry->setIntersectionProgram(context->createProgramFromPTXFile(path, "intersect"));
	optixGeometry["parallelogram"]->setUserData(sizeof(Parallelogram), this);
	return optixGeometry;
}

RT_FUNCTION float  Parallelogram::GetArea() const {

	float3 side1 = p1 - p0;
	float3 side2 = p2 - p0;
	return length(cross(side1, side2));

}