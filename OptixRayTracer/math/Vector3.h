#pragma once

#include "core/optix_global.h"


float ValueAt(float3 v, int i) {
	if (i == 0)
		return v.x;
	if (i == 1)
		return v.y;
	return v.z;
}


float2 ProjectVec3DTo2D(float3 vec, int u, int v) {

	return make_float2(ValueAt(vec, u), ValueAt(vec, v));

}

void GetProjectionAxes(const float3 &normal, int &u, int &v) {

	u = 0; v = 1;
	if (abs(normal.y) > abs(normal.x) &&
		abs(normal.y) > abs(normal.z)) {
		u = 0;
		v = 2;
	}
	else if (abs(normal.x) > abs(normal.y) &&
		abs(normal.x) > abs(normal.z)) {
		u = 1;
		v = 2;
	}

}