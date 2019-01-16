#include "core/optix_global.h"
#include "core/Ray.h"
#include "shapes/Triangle.h"
#include "core/math.h"
#include <optix_world.h>

using namespace optix;


rtBuffer<float3> vertexBuffer;
rtBuffer<float3> normalBuffer;
rtBuffer<int3> indexBuffer;
rtDeclareVariable(HitRecord, hit, attribute hit, );
rtDeclareVariable(Ray, ray, rtCurrentRay, );
//rtDeclareVariable(float3, geometricNormal, attribute geometricNormal, );
//rtDeclareVariable(float3, shadingNormal, attribute shadingNormal, );



RT_PROGRAM void intersect(int primIdx) {

	int3 index = indexBuffer[primIdx];

	float3 p0 = vertexBuffer[index.x];
	float3 p1 = vertexBuffer[index.y];
	float3 p2 = vertexBuffer[index.z];

	float3 E1 = p1 - p0;
	float3 E2 = p2 - p0;

	float3 S1 = cross(ray.direction, E2);
	float invDivisor = 1.0f / dot(S1, E1);

	float3 T = ray.origin - p0;
	float u = dot(T, S1) * invDivisor;

	float3 S2 = cross(T, E1);
	float v = dot(ray.direction, S2) * invDivisor;

	if (u < 0.0f || v < 0.0f || u + v > 1.0f) return;

	float t = dot(E2, S2) * invDivisor;
	if (rtPotentialIntersection(t)) {
		float eps = 0.001;
		t = t < eps ? t : t - eps;
		float3 p = ray.origin + ray.direction * t;
		//p = p0 + u*E1 + v*E2;
		float3 normal = normalize(cross(E1, E2));
		//rtPrintf("(%f, %f, %f)\n", normal.x, normal.y, normal.z);
		hit.position = p;
		hit.normal = normal;
		rtReportIntersection(0);

	}

}


/*
RT_PROGRAM void intersect(int primIdx) {
	
	int3 index = indexBuffer[primIdx];

	double3 p0 = ToDouble(vertexBuffer[index.x]);
	double3 p1 = ToDouble(vertexBuffer[index.y]);
	double3 p2 = ToDouble(vertexBuffer[index.z]);
	
	double3 E1 = p1 - p0;
	double3 E2 = p2 - p0;

	double3 S1 = cross(ToDouble(ray.direction), E2);
	double invDivisor = 1.0 / dot(S1, E1);

	double3 T = ToDouble(ray.origin) - p0;
	double u = dot(T, S1) * invDivisor;

	double3 S2 = cross(T, E1);
	double v = dot(ToDouble(ray.direction), S2) * invDivisor;

	if (u < 0.0 || v < 0.0 || u + v > 1.0) return;

	float t = (float)dot(E2, S2) * invDivisor;
	if (rtPotentialIntersection(t)) {
		float eps = 0.001;
		t = t < eps? t : t - eps;
		double3 p = ToDouble(ray.origin) + ToDouble(ray.direction) * t;
		double3 normal = normalize(cross(E1, E2));
		//rtPrintf("(%f, %f, %f)\n", normal.x, normal.y, normal.z);
		hit.position = ToFloat(p);
		hit.normal = ToFloat(normal);
		rtReportIntersection(0);

	}

}*/

RT_PROGRAM void boundingBox(int primIdx, float result[6]) {

	const int3 index = indexBuffer[primIdx];

	const float3 p0 = vertexBuffer[index.x];
	const float3 p1 = vertexBuffer[index.y];
	const float3 p2 = vertexBuffer[index.z];

	optix::Aabb* aabb = (optix::Aabb*) result;
	aabb->m_min = fminf(fminf(p0, p1), p2);
	aabb->m_max = fmaxf(fmaxf(p0, p1), p2);

}



