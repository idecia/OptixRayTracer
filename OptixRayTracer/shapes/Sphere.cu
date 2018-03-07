#include "core/optix_global.h"
#include "core/Ray.h"
#include "shapes/Sphere.h"

#include <optix_world.h>

using namespace optix;


rtDeclareVariable(Sphere, sphere, , );
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );

RT_FUNCTION void setHitRecord(float t) {

	hit.position = ray.origin + t * ray.direction;
	hit.normal = sphere.NormalAt(hit.position);

}

RT_PROGRAM void intersect(int primIdx) {
		
	float3 center = sphere.GetCenter();
	float radius  = sphere.GetRadius();
	float3 O = ray.origin - center;
	float b = dot(O, ray.direction);
	float c = dot(O, O) - radius * radius;
	float discriminant = b * b - c;
	if (discriminant > 0.0f) {
		float sqrtDiscriminant = sqrtf(discriminant);
		float tMin = (-b - sqrtDiscriminant);
		bool checkSecond = true;
		if (rtPotentialIntersection(tMin)) {
			setHitRecord(tMin);
			if (rtReportIntersection(0)) 
				checkSecond = false;
		}
		if (checkSecond) {
			float tMax = (-b + sqrtDiscriminant);
			if (rtPotentialIntersection(tMax)) {
				setHitRecord(tMax);
				rtReportIntersection(0);
			}
		}
	}
}

RT_PROGRAM void boundingBox(int, float result[6]) {

	float3 center = sphere.GetCenter();
    float3 radius3f = make_float3(sphere.GetRadius());
	optix::Aabb* aabb = (optix::Aabb*) result;
    aabb->m_min = center - radius3f;
    aabb->m_max = center + radius3f;
}
