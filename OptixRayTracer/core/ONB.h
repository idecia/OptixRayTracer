<<<<<<< HEAD
=======

>>>>>>> d11640ade35c065cb0888b8a31c301286d7d5ea8
#pragma once

#include "core/optix_global.h"

class ONB {

public:

	RT_FUNCTION ONB();
	RT_FUNCTION ONB(float3 normal);
	RT_FUNCTION float3 WorldToLocal(const float3 &v);
	RT_FUNCTION float3 LocalToWorld(const float3 &v);

private:

	float3 n;
	float3 s;
	float3 t;


};


RT_FUNCTION ONB::ONB() { }

RT_FUNCTION ONB::ONB(float3 normal)
	: n(normal) {

	if (fabs(n.x) > fabs(n.z)) {
		s.x = -n.y;
		s.y = n.x;
		s.z = 0;
<<<<<<< HEAD
	}
	else {
=======
	} else {
>>>>>>> d11640ade35c065cb0888b8a31c301286d7d5ea8
		s.x = 0;
		s.y = -n.z;
		s.z = n.y;
	}

	s = normalize(s);
	t = cross(s, n);
<<<<<<< HEAD
	
=======
>>>>>>> d11640ade35c065cb0888b8a31c301286d7d5ea8

}


RT_FUNCTION float3 ONB::WorldToLocal(const float3 &v) {

	return make_float3(dot(v, s), dot(v, t), dot(v, n));

}


RT_FUNCTION float3 ONB::LocalToWorld(const float3 &v) {

<<<<<<< HEAD
	return  (v.x * s) + (v.y * t) + (v.z * n);

=======
	return  (v.x * s) +(v.y * t) + (v.z * n);
	 
>>>>>>> d11640ade35c065cb0888b8a31c301286d7d5ea8
}