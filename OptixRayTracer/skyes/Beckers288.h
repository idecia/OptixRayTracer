#pragma once
#include "core/optix_global.h"
#include <iostream>
#include <vector>
using namespace std;

static RT_FUNCTION int beckers(const float3 &v) {

	if (v.z <= 0.0)
		return 0;

	int n = 11;
	int rings[]         = { 1, 8, 13, 19, 24, 29, 34, 37, 40, 41, 41 };
	int ringsacum[]     = { 1, 9, 22, 41, 65, 94, 128, 165, 205, 246, 288 };
	float ringsangles[] = { 0.1001, 0.2438, 0.3904, 0.5374, 0.6851, 0.8330,
		                    0.9806, 1.1281, 1.2755, 1.4231, 1.5708 };

	float azim  = atan2(v.x, v.y);
	float zenit = atan2(sqrt(v.x*v.x + v.y*v.y), v.z);

	int l = 0;
	int r = n-1;
	int mid = floor((l + r) / 2);
	while (l < r) {
		if (zenit > ringsangles[mid])
			l = mid + 1;
		else if (zenit < ringsangles[mid])
			r = mid;
		else
			break;
		mid = floor((l + r) / 2);
	}

	int patch;
	int ringidx = mid;
	if (ringidx == 0)
		patch = 1;
	else {
		float anglestep = 2*M_PIf / rings[ringidx];
		int ringint = floor(azim / anglestep);
		patch = ringsacum[ringidx-1] + ringint;
	}

	return patch;

}

