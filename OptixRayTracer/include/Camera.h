#pragma once

#include <optix_world.h>

using namespace optix;

class Camera {

public:

	float3 eye;
	float3 lookAt;
	float3 up;
	float3 U, V, W;
	float distance;

	Camera(float3 eye, float3 lookAt, float3 up, float distance);
	RT_HOSTDEVICE Ray GenerateRay(const float2 &sample);

private:

	void ComputeUVW(float3 lookAt, float3 up, float distance);

};
