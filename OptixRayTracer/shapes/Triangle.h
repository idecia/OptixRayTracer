#pragma once

#include "core/optix_global.h"
#include "core/Ray.h"

class Triangle {

public:

	RT_FUNCTION Triangle();
	RT_FUNCTION Triangle(const float3 &a, const float3 &b, const float3 &c);
	RT_FUNCTION Triangle(const float3 &a, const float3 &b, const float3 &c, bool flipNormal);
	RT_FUNCTION float3 NormalAt(const float3 &point) const;
	RT_FUNCTION float  GetArea() const;
	RT_FUNCTION float3 Sample(const float2 &sample);
	RT_FUNCTION_HOST Geometry GetOptixGeometry(Context context);


private:

	float3 a, b, c;
	float3 normal;

};


RT_FUNCTION Triangle::Triangle() { }

RT_FUNCTION Triangle::Triangle(const float3 &a, const float3 &b, const float3 &c, bool flipNormal)
	: a(a), b(b), c(c) {

	int orientation = flipNormal ? 1 : -1;
	normal = orientation * normalize(cross(b - a, c - a));
}

RT_FUNCTION Triangle::Triangle(const float3 &a, const float3 &b, const float3 &c)
	: a(a), b(b), c(c) {
	
	normal = normalize(cross(b - a, c - a));

}

RT_FUNCTION float3 Triangle::NormalAt(const float3 &point) const {

	return  normal;

}

RT_FUNCTION float3 Triangle::Sample(const float2 &sample) {

	float2 b = UniformTriangleSample(sample.x, sample.y);
	return (1.0f - b[0] - b[1])*a + b[1]*b + b[2]*c;

}

RT_FUNCTION float  Triangle::GetArea() const {

	float3 e1 = b - a;
	float3 e2 = c - a;
	return 0.5f * length(cross(e1, e2));

}
