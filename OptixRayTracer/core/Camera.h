#pragma once

#include "core/optix_global.h"
#include "core/Ray.h"


class Camera {

public:

	RT_FUNCTION Camera();

	RT_FUNCTION Camera(const float3 &eye, const float3 &lookAt, const float3 &up, float distance);
	
	RT_FUNCTION Ray GenerateRay(const float2 &sample) const;

private:

	float3 eye;
	float3 lookAt;
	float3 up;
	float3 U, V, W;
	float distance;

	RT_FUNCTION void ComputeUVW(const float3 &lookAt, const float3 &up, float distance);

};


RT_FUNCTION Camera::Camera() {}

RT_FUNCTION Camera::Camera(const float3 &eye, const float3 &lookAt, const float3 &up, float distance)
	: eye(eye), lookAt(lookAt), up(up), distance(distance) {

	ComputeUVW(eye, lookAt, up);

}

RT_FUNCTION Ray Camera::GenerateRay(const float2 &sample) const {

	float3 dir = sample.x * U + sample.y * V - distance * W;
	dir = normalize(dir);
	Ray ray = make_Ray(eye, dir, RADIANCE_RAY, 0.05f, RT_DEFAULT_MAX);
	return ray;
}

RT_FUNCTION void Camera::ComputeUVW(const float3 &lookAt, const float3 &up, float distance) {

	W = normalize(eye - lookAt);
	U = normalize(cross(up, U));
	V = cross(W, U);

}

