#pragma once

#include "core/optix_global.h"

typedef enum {
	CW,
	CCW,
	NONE
} WindingType;


WindingType TriangleWinding(float2 a, float2 b, float2 c);
void Barycentric(float2 a, float2 b, float2 c, float2 p, float &u, float &v, float &w);
bool TestPointTriangle(float2 p, float2 a, float2 b, float2 c);



float TriangleArea2D(float2 a, float2 b, float2 c) {

	return ( (a.x - c.x) * (b.y - c.y) - 
		     (a.y - c.y) * (b.x - c.x) ) * 0.5f;
}

WindingType TriangleWinding(float2 a, float2 b, float2 c) {

	float area = TriangleArea2D(a, b, c);
	return (area > 0.0f) ? WindingType::CCW : WindingType::CW;
	
}


void Barycentric(float2 a, float2 b, float2 c, float2 p, float &u, float &v, float &w) {

	float2 v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = dot(v0, v0);
	float d01 = dot(v0, v1);
	float d11 = dot(v1, v1);
	float d20 = dot(v2, v0);
	float d21 = dot(v2, v1);
	float invDenom = 1.0f / (d00 * d11 - d01 * d01);
	v = (d11 * d20 - d01 * d21) * invDenom;
	w = (d00 * d21 - d01 * d20) * invDenom;
	u = 1.0f - v - w;

}

bool TestPointTriangle(float2 a, float2 b, float2 c, float2 p) {
	
	float u, v, w;
	Barycentric(a, b, c, p, u, v, w);
	return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
}

