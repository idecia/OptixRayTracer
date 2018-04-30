#include "core/optix_global.h"

RT_FUNCTION float3 UniformHemisphereSample(float u1, float u2) {

	float3 direction;
	float cosPhi = cosf(2.0f * M_PIf * u1);
	float sinPhi = sinf(2.0f * M_PIf * u1);
	float cosTheta = 1.0f - u2;
	float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
	direction.x = sinTheta * cosPhi;
	direction.y = sinTheta * sinPhi;
	direction.z = cosTheta;
	return direction;

}
