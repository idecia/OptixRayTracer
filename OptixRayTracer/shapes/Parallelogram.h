#pragma once

#include "core/optix_global.h"

#include <optix_world.h>
#include <optixu/optixpp_namespace.h>
#include "core/Ray.h"
using namespace optix;

class Parallelogram{

public:

	RT_FUNCTION Parallelogram();
	RT_FUNCTION Parallelogram(const float3 &p0, const float3 &p1, const float3 &p2);
	RT_FUNCTION float3 NormalAt(const float3 &point) const;
	RT_FUNCTION float3 GetP0() const;
	RT_FUNCTION float3 GetP1() const;
	RT_FUNCTION float3 GetP2() const;
	RT_FUNCTION float4 GetPlane() const;
	RT_FUNCTION_HOST Geometry GetOptixGeometry(Context context);


private:

	float3 p0, p1, p2;
	float4 plane;  

};


RT_FUNCTION Parallelogram::Parallelogram() { }

RT_FUNCTION Parallelogram::Parallelogram(const float3 &p0, const float3 &p1, const float3 &p2, bool ccw)
	: p0(p0), p1(p1), p2(p2) {

	float3 normal = normalize(cross(p1 - p0, p2 - p0));
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



