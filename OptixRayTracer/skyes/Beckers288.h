#pragma once
#include "core/optix_global.h"
#include <iostream>
#include <vector>
using namespace std;

static RT_FUNCTION int beckers(const float3 &v) {

	if (v.z <= 0.0)
		return 0;

	int nRings = 6;
	//int nRings = 8;
		//int nRings = 41;
		//int nRings = 11;
		//int nRings = 57;
	
	int rings[] = { 1, 4, 11, 16, 19, 21 };
   // int rings[] = { 1, 6, 12, 18, 22, 26, 29, 30 };
	//int rings[]      = { 1, 8, 13, 19, 24, 29, 34, 37, 40, 41, 42 };
	//int rings[] = { 1, 3, 10, 16, 23, 29, 35, 41, 47, 53, 59, 65, 70, 76, 82, 87, 92, 97, 102, 107, 111, 116, 120, 124, 128, 132, 135, 138, 142, 144,147,149,152,154,155,157,158,159,160,160,160 };
	/*int rings[] = {  1,   9,  14,  20,  27,  33,  39,  45,  51,  57,  63,  69,  75,  81,  87,  93,  98, 104, 110, 115, 120,
		           126, 131, 136, 141, 146, 151, 155, 160, 164, 168, 173, 177, 181, 184, 188, 191, 195, 198, 201, 204, 206, 
				   209, 211, 213, 215, 217, 219, 221, 222, 223, 224, 225, 226, 226, 227, 227 };*/
	//int ringsacum[] = { 1, 4, 14, 30, 53, 82, 117, 158, 205, 258, 317, 382, 452, 528, 610, 697, 789, 886, 988, 1095, 1206, 1322, 1442, 1566, 1694, 1826, 1961, 2099, 2241, 2385, 2532,2681,2833,2987,3142,3299,3457,3616,3776,3936,4096 };
	//int ringsacum[]  = { 1, 9, 22, 41, 65, 94, 128, 165, 205, 246, 288 };
	//int ringsacum[] = { 1, 7, 19, 37, 59, 85, 114, 144 };
	int ringsacum[] = { 1, 5, 16, 32, 51, 72 };

	/*int ringsacum[] = {    1,   10,   24,   44,   71,  104,  143,  188,  239,  296,
						 359,  428,  503,  584,  671,  764,  862,  966, 1076, 1191, 
				     	1311, 1437, 1568, 1704, 1845, 1991, 2142, 2297, 2457, 2621,
						2789, 2962, 3139, 3320, 3504, 3692, 3883, 4078, 4276, 4477,
						4681, 4887, 5096, 5307, 5520, 5735, 5952, 6171, 6392, 6614, 
						6837, 7061, 7286, 7512, 7738, 7965, 8192 };*/
	float ringslat[] = { 0.0822, 0.3874, 0.6850, 0.9799, 1.2754, 1.5708 };
	//float ringslat[] = { 0.1044, 0.3137, 0.5246, 0.7336, 0.9433, 1.1529, 1.3619, 1.5708 };
	//float ringslat[] = { 0.1001, 0.2438, 0.3904, 0.5374, 0.6851, 0.8330, 0.9806, 1.1281, 1.2755, 1.4231, 1.5708 };
	//float ringslat[] = { 0.0049, 0.0434, 0.0825, 0.1219, 0.1611, 0.2002, 0.2393, 0.2785, 0.3176, 0.3568, 0.3959, 0.4351, 0.4742, 0.5134, 0.5526, 0.5917, 0.6309, 0.6700, 0.7092, 0.7483, 0.7875, 0.8266, 0.8658, 0.9050, 0.9441, 0.9833, 1.0225, 1.0616, 1.1008, 1.1400, 1.1791, 1.2183, 1.2575, 1.2966, 1.3358, 1.3750,
	//	1.4141,1.4533,1.4925,1.5316,1.5708 };
	/*float ringslat[] = { 0.0213, 0.0485, 0.0759, 0.1036, 0.1312, 0.1587, 0.1863, 0.2139, 0.2415, 0.2691, 0.2968, 0.3245,
		                 0.3522, 0.3799, 0.4076, 0.4353, 0.4630, 0.4907, 0.5184, 0.5461, 0.5738, 0.6015, 0.6292, 0.6569,
		                 0.6846, 0.7123, 0.7400, 0.7677, 0.7954, 0.8231, 0.8508, 0.8784, 0.9061, 0.9338, 0.9615, 0.9892,
		                 1.0169, 1.0446, 1.0723, 1.1000, 1.1277, 1.1554, 1.1831, 1.2108, 1.2385, 1.2662, 1.2938, 1.3215,
	                 1.3492, 1.3769, 1.4046, 1.4323, 1.4600, 1.4877, 1.5154, 1.5431, 1.5708 };*/
	
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

