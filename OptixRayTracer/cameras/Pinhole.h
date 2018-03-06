#pragma once

#include "core/optix_global.h"

class Pinhole {

public:

	float3 eye;
	float3 lookAt;
	float3 up;
	float3 U, V, W;
	float distance;

	RT_FUNCTION Pinhole();
	RT_FUNCTION Pinhole(const float3 &eye, const float3 &lookAt, const float3 &up, float distance);
	RT_FUNCTION Ray GenerateRay(const float2 &sample) const;
	RT_FUNCTION_HOST void OptixSetup(Context context);

private:

	void ComputeUVW();

};


RT_FUNCTION Pinhole::Pinhole() {}

RT_FUNCTION Pinhole::Pinhole(const float3 &eye, const float3 &lookAt, const float3 &up, float distance)
	: eye(eye), lookAt(lookAt), up(up), distance(distance) {

	ComputeUVW();

}

RT_FUNCTION Ray Pinhole::GenerateRay(const float2 &sample) const {

	float3 dir = sample.x * U + sample.y * V - distance * W;
	dir = normalize(dir);
	Ray ray = make_Ray(eye, dir, RayType::RADIANCE, 0.05f, RT_DEFAULT_MAX);
	return ray;
}

RT_FUNCTION void Pinhole::ComputeUVW() {

	W = normalize(eye - lookAt);
	U = normalize(cross(up, W));
	V = cross(W, U);

}

RT_FUNCTION_HOST void Pinhole::OptixSetup(Context context) {

	const char* path = "./Pinhole.ptx";
	Program program = context->createProgramFromPTXFile(path, "pinhole");
	context->setRayGenerationProgram(0, program);
	path = "./Whitted.ptx";
	Program miss = context->createProgramFromPTXFile(path, "miss");
	context->setMissProgram(RayType::RADIANCE, miss);
	//TODO: Exception program 
	context["camera"]->setUserData(sizeof(Pinhole), this);
}
