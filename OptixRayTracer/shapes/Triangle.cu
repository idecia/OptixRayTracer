#include "core/optix_global.h"
#include "core/Ray.h"
#include "shapes/Triangle.h"
#include <optix_world.h>

using namespace optix;


rtDeclareVariable(Triangle, triangle, , );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );


RT_PROGRAM void intersect(int primIdx) {

	float3 p0 = triangle.GetP0();
	float3 p1 = triangle.GetP1();
	float3 p2 = triangle.GetP2();

	float3 E1 = p1 - p0;
	float3 E2 = p2 - p0;
	operator-(p1, p2);

	float3 S1 = cross(ray.direction, E2);
	float invDivisor = 1.0f / dot(S1, E1);

	float3 T = ray.origin - p0;
	float u = dot(T, S1) * invDivisor;
	
	float3 S2 = cross(T, E1);
	float v = dot(ray.direction, S2) * invDivisor;

	if (u < 0.0f || v < 0.0f || u + v > 1.0f) return;

	float t = dot(E2, S2) * invDivisor;

	if (rtPotentialIntersection(t)) {
		float3 p = ray.origin + ray.direction * t;
		hit.normal = triangle.NormalAt(p);
		hit.position = p + 0.5*hit.normal;
		rtReportIntersection(0);

	}



}

RT_PROGRAM void boundingBox(int, float result[6]) {

	float3 p0 = triangle.GetP0();
	float3 p1 = triangle.GetP1();
	float3 p2 = triangle.GetP2();

	optix::Aabb* aabb = (optix::Aabb*) result;
	aabb->m_min = fminf(fminf(p0, p1), p2);
	aabb->m_max = fmaxf(fmaxf(p0, p1), p2);

}



