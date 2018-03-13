#include "core/optix_global.h"
#include "core/Ray.h"
#include "shapes/Parallelogram.h"
#include <optix_world.h>

using namespace optix;


rtDeclareVariable(Parallelogram, parallelogram, , );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );


RT_PROGRAM void intersect(int primIdx) {

	float4 plane = parallelogram.GetPlane();
	float3 normal = make_float3(plane);

	float nDotD = dot(normal, ray.direction);
	float nDotO = dot(normal, ray.origin);
	float t = (plane.w - nDotO) / nDotD;

	if (t <= ray.tmin || t >= ray.tmax) return;

	float3 p = ray.origin + ray.direction * t;
	float3 p0 = parallelogram.GetP0();
	float3 p1 = parallelogram.GetP1();
	float3 p2 = parallelogram.GetP2();

	float3 side1 = p1 - p0;// normalize(p1 - p0);
	float u = dot(p - p0, side1) / dot (side1, side1);
	if (u < 0 || u > 1) return;


	float3 side2 = p2 - p0;//normalize(p2 - p0);
	float v = dot(p - p0, side2) / dot(side2, side2);
	if (v < 0 || v > 1) return;

	if (rtPotentialIntersection(t)) {
		hit.position = p;
		hit.normal = normal;
		rtReportIntersection(0);
	}


}

RT_PROGRAM void boundingBox(int, float result[6]) {

	float3 p0 = parallelogram.GetP0();
	float3 p1 = parallelogram.GetP1();
	float3 p2 = parallelogram.GetP2();
	float3 p3 = p0 + p1 + p2;

	optix::Aabb* aabb = (optix::Aabb*) result;
	aabb->m_min = fminf(fminf(p0, p1), fminf(p2, p3));
	aabb->m_max = fmaxf(fmaxf(p0, p1), fmaxf(p2, p3));

}
