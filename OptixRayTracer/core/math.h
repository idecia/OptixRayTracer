#pragma once
#include "core/optix_global.h"


RT_FUNCTION  double fmax(const double3& a) {
	return max(a.x, max(a.y, a.z));
}


RT_FUNCTION double3 ToDouble(const float3 &v) {
	double3 vd;
	vd.x = (double)v.x;
	vd.y = (double)v.y;
	vd.z = (double)v.z;
	return vd;
}

RT_FUNCTION float3 ToFloat(const double3 &v) {
	float3 vf;
	vf.x = (float)v.x;
	vf.y = (float)v.y;
	vf.z = (float)v.z;
	return vf;
}

RT_FUNCTION double3 cross(const double3& a, const double3& b) {
	return make_double3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

RT_FUNCTION  double dot(const double3& a, const double3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

RT_FUNCTION  double3 normalize(const double3& v) {
	double3 vn;
	double invLen = 1.0 / sqrt(dot(v, v));
	vn.x = v.x * invLen;
	vn.y = v.y * invLen;
	vn.z = v.z * invLen;
	return vn;
}

RT_FUNCTION  double3 operator-(const double3& a, const double3& b) {
	return make_double3(a.x - b.x, a.y - b.y, a.z - b.z);
}

RT_FUNCTION  double3 operator+(const double3& a, const double3& b) {
	return make_double3(a.x + b.x, a.y + b.y, a.z + b.z);
}


RT_FUNCTION  double3 operator*(const double3& a, const double t) {
	return make_double3(a.x*t, a.y*t, a.z*t);

}

RT_FUNCTION  double3 operator/(const double3& a, const double t) {
	double invt = 1.0 / t;
	return make_double3(a.x*invt, a.y*invt, a.z*invt);

}

RT_FUNCTION  double3 operator*(const double3& a, const double3& b) {
	return make_double3(a.x*b.x, a.y*b.y, a.z*b.z);

}
