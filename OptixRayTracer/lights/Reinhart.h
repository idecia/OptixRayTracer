#pragma once
#include "core/optix_global.h"
#include <iostream>
#include <vector>
using namespace std;

static RT_FUNCTION  float mod(float a, float m) {

	return a - m *floorf(a / m);
}

static RT_FUNCTION int reinhart(const float3 &v, int m) {

	if (v.z <= 0.0)
		return 0;
	int rings1[]     = { 30, 30, 24, 24, 18, 12, 6, 1 };
	int ringsacum1[] = { 0, 30, 60, 84, 108, 126, 138, 144 };
	float elevstep = M_PIf / (2.0f * (7.0f*m + 0.5f));
	
	float azim = atan2(v.x, v.y);

	float elev = atan2(v.z, sqrtf(v.x*v.x + v.y*v.y));
	
	int ring1 = max((int)ceil(elev / (elevstep*m)), 1);
	
	int ringm = max((int)ceil(elev / (elevstep)), 1);
	
	int inring = (int) ceil(mod(azim + M_PIf / (rings1[ring1-1]*m), 2.0f*M_PIf) / (2.0f*M_PIf / (rings1[ring1-1]*m)));
	
	int patch = m*(ringsacum1[ring1-1]*m + rings1[ring1-1]*((ringm - 1) - (ring1 - 1)*m)) + inring;
	return patch;

}