#include "Camera.h"
#include "Ray.h"


Camera::Camera(float3 eye, float3 lookAt, float3 up, float distance)
	: eye(eye), lookAt(lookAt), up(up), distance(distance) {

	ComputeUVW(eye, lookAt, up);

}


void Camera::ComputeUVW(const float3 &eye, const float3 &lookAt, const float3 &up) {

	W = normalize(eye - lookAt);
	U = normalize(cross(up, U));
	V = cross(W, U);

}

RT_HOSTDEVICE Ray Camera::GenerateRay(const float2 &sample) {

	float3 dir = sample.x * U + sample.y * V - distance * W;
	dir = normalize(dir);
	Ray ray = make_Ray(eye, dir, RADIANCE_RAY , 0.05f, RT_DEFAULT_MAX);
	return ray;
}

