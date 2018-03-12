#include "core/optix_global.h"
#include "core/Ray.h"
#include "shapes/Sphere.h"

#include <optix_world.h>

using namespace optix;


rtDeclareVariable(Parallelogram, parallelogram, , );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );


RT_PROGRAM void intersect(int primIdx) {

	float4 plane = parallelogram.getPlane();
	float3 normal = make_float3(plane);

	float nDotD = normal * ray.direction;
	float nDotO = dot(normal, ray.origin);
	float t = (plane.w - nDotO) / nDotD;

	if (t <= ray.tmin && t >= ray.tmax) return;

	float3 p = ray.origin + ray.direction * t;
	float3 p0 = parallelogram.getP0();
	float3 p1 = parallelogram.getP1();
	float3 p2 = parallelogram.getP2();

	float3 side1 = normalize(p1 - p0);
	float u = dot(side1, p - p0);
	if (u < 0 && u > 1) return;

	float3 side2 = normalize(p2 - p0);
	float v = dot(side2, p - p0);
	if (v < 0 && v > 1) return;

	if (rtPotentialIntersection(t)) {
		hit.position = p
		hit.normal = normal;
		rtReportIntersection(0);
	}


}

RT_PROGRAM void boundingBox(int, float result[6]) {

	float3 p = ray.origin + ray.direction * t;
	float3 p0 = parallelogram.getP0();
	float3 p1 = parallelogram.getP1();
	float3 p2 = parallelogram.getP2();
	float3 p3 = p0 + p1 + p2;

	optix::Aabb* aabb = (optix::Aabb*) result;
	aabb->m_min = fminf(fminf(p0, p1), fminf(p2, p3));
	aabb->m_max = fmaxf(fmaxf(p0, p1), fmaxf(p2, p3));

}
