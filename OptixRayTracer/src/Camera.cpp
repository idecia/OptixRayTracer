#include "Camera.h"


Camera::Camera(float3 eye, float3 lookAt, float3 up, float distance)
	: eye(eye), lookAt(lookAt), up(up), distance(distance) {

	computeUVW(eye, lookAt, up);

}


void Camera::computeUVW(const float3 &eye, const float3 &lookAt, const float3 &up) {

	W = normalize(eye - lookAt);
	U = normalize(cross(up, U));
	V = cross(W, U);

}

RT_HOSTDEVICE Ray Camera::generateRay(const float2 &sample) {

	float3 dir = sample.x * U + sample.y * V - distance * W;
	dir = normalize(dir);
	Ray ray = make_Ray(eye, dir, 0, 0.05f, RT_DEFAULT_MAX);
	return ray;
}

