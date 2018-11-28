#pragma once

#include "core/optix_global.h"
#include "samplers/Util.h"
#include "core/Ray.h"

class Triangle {

public:

	RT_FUNCTION Triangle();
	RT_FUNCTION Triangle(const float3 &p0, const float3 &p1, const float3 &p2, bool flipNormal);
	RT_FUNCTION Triangle(const float3 &p0, const float3 &p1, const float3 &p2);
	RT_FUNCTION float3 NormalAt(const float3 &point) const;
	RT_FUNCTION float3 GetP0() const;
	RT_FUNCTION float3 GetP1() const;
	RT_FUNCTION float3 GetP2() const;
	RT_FUNCTION float  GetArea() const;
	RT_FUNCTION float3 Sample(const float2 &sample);
	RT_FUNCTION_HOST Geometry GetOptixGeometry(Context context);


private:

	float3 p0, p1, p2;
	float3 normal;

};


RT_FUNCTION Triangle::Triangle() { }

RT_FUNCTION Triangle::Triangle(const float3 &p0, const float3 &p1, const float3 &p2, bool flipNormal)
	: p0(p0), p1(p1), p2(p2) {

	int orientation = flipNormal ? 1 : -1;
	normal = orientation * normalize(cross(p1 - p0, p2 - p0));
}

RT_FUNCTION Triangle::Triangle(const float3 &p0, const float3 &p1, const float3 &p2)
	: p0(p0), p1(p1), p2(p2) {
	
	normal = normalize(cross(p1 - p0, p2 - p0));

}

RT_FUNCTION float3 Triangle::NormalAt(const float3 &point) const {

	return  normal;

}

RT_FUNCTION float3 Triangle::GetP0() const {

	return p0;
}

RT_FUNCTION float3 Triangle::GetP1() const {

	return p1;
}

RT_FUNCTION float3 Triangle::GetP2() const {

	return p2;
}

RT_FUNCTION float3 Triangle::Sample(const float2 &sample) {

	float2 b = UniformTriangleSample(sample.x, sample.y);
	return (1.0f - b.x - b.y)*p0 + b.x*p1 + b.y*p2;

}


RT_FUNCTION_HOST Geometry Triangle::GetOptixGeometry(Context context) {

	Geometry optixGeometry = context->createGeometry();
	optixGeometry->setPrimitiveCount(1u);
	const char* path = "./Triangle.ptx";
	optixGeometry->setBoundingBoxProgram(context->createProgramFromPTXFile(path, "boundingBox"));
	optixGeometry->setIntersectionProgram(context->createProgramFromPTXFile(path, "intersect"));
	optixGeometry["triangle"]->setUserData(sizeof(Triangle), this);
	return optixGeometry;
}


RT_FUNCTION float  Triangle::GetArea() const {

	float3 e1 = p1 - p0;
	float3 e2 = p2 - p0;
	return 0.5f * length(cross(e1, e2));

}
