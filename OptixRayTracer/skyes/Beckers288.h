#pragma once
#include "core/optix_global.h"
#include <iostream>
#include <vector>
using namespace std;

static RT_FUNCTION int beckers(const float3 &v) {

	if (v.z <= 0.0)
		return 0;

	//int nRings = 41;
	int nRings = 11;
	int rings[]      = { 1, 8, 13, 19, 24, 29, 34, 37, 40, 41, 42 };
	//int rings[] = { 1, 3, 10, 16, 23, 29, 35, 41, 47, 53, 59, 65, 70, 76, 82, 87, 92, 97, 102, 107, 111, 116, 120, 124, 128, 132, 135, 138, 142, 144,147,149,152,154,155,157,158,159,160,160,160 };
	//int ringsacum[] = { 1, 4, 14, 30, 53, 82, 117, 158, 205, 258, 317, 382, 452, 528, 610, 697, 789, 886, 988, 1095, 1206, 1322, 1442, 1566, 1694, 1826, 1961, 2099, 2241, 2385, 2532,2681,2833,2987,3142,3299,3457,3616,3776,3936,4096 };
	int ringsacum[]  = { 1, 9, 22, 41, 65, 94, 128, 165, 205, 246, 288 };
	float ringslat[] = { 0.1001, 0.2438, 0.3904, 0.5374, 0.6851, 0.8330,
		            //     0.9806, 1.1281, 1.2755, 1.4231, 1.5708 };
	//float ringslat[] = { 0.0049, 0.0434, 0.0825, 0.1219, 0.1611, 0.2002, 0.2393, 0.2785, 0.3176, 0.3568, 0.3959, 0.4351, 0.4742, 0.5134, 0.5526, 0.5917, 0.6309, 0.6700, 0.7092, 0.7483, 0.7875, 0.8266, 0.8658, 0.9050, 0.9441, 0.9833, 1.0225, 1.0616, 1.1008, 1.1400, 1.1791, 1.2183, 1.2575, 1.2966, 1.3358, 1.3750,
		1.4141,1.4533,1.4925,1.5316,1.5708 };
	float azim  = atan2(v.y, v.x);
	if (azim < 0)
		azim = 2*M_PIf + azim;
	float zenit = atan2(sqrt(v.x*v.x + v.y*v.y), v.z);
    int l = 0;
	int r = nRings - 1;
	int mid = (int)floorf((l + r) / 2);
	while (l < r) {
		if (zenit > ringslat[mid])
			l = mid + 1;
		else if (zenit < ringslat[mid])
			r = mid;
		else
			break;
		mid = (int)floorf((l + r) / 2);
	}
	int ringid = mid;

	int patch;
	if (ringid == 0)
		patch = 1;
	else {
		float anglestep = 2*M_PIf / rings[ringid];
		//cuidado aqui con overflow
		int ringint = (int) ceil(azim / anglestep);
		patch =  ringsacum[ringid-1] + ringint;
	}
	return patch;

}

