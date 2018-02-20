#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

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
	RT_HOSTDEVICE Ray generateRay(const float2 &sample);

private:

	void computeUVW(float3 lookAt, float3 up, float distance);

};

#endif