#pragma once

#include "core/optix_global.h"
#include "geometry/Polygon2D.h"
#include "geometry/Polygon3D.h"

enum WindingType {
	CW,
	CCW,
	NONE
};

WindingType TriangleWinding(float2 a, float2 b, float2 c);
float AreaParallelogram(float2 a, float2 b, float2 c);
WindingType PolygonWinding(const Polygon2D &poly);
float PolygonArea(const Polygon2D &poly);
void Barycentric(float2 a, float2 b, float2 c, float2 p, float &u, float &v, float &w);
bool TestPointTriangle(float2 p, float2 a, float2 b, float2 c);
float3 NewellNormal(Polygon3D poly);


float TriangleArea2D(float2 a, float2 b, float2 c) {

	return ( (a.x - c.x) * (b.y - c.y) - 
		     (a.y - c.y) * (b.x - c.x) ) * 0.5f;
}

WindingType TriangleWinding(float2 a, float2 b, float2 c) {

	float area = TriangleArea2D(a, b, c);
	return area > 0 ? CCW : CW;
	
}

float PolygonArea2D(const Polygon2D &poly) {

	const vector<float2> v = poly.GetVertices();
	int n = v.size();
	float area = 0.0f;
	for (int i = 0, j = n - 1; i < n; j = i, i++) {
		area += (v[i].x + v[j].x) * (v[i].y - v[j].y);
	}
	return 0.5 * area;

}

WindingType PolygonWinding(const Polygon2D &poly) {

	float area = PolygonArea2D(poly);
	return area > 0 ? CCW : CW;

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

bool TestPointTriangle(float2 p, float2 a, float2 b, float2 c) {
	
	float u, v, w;
	Barycentric(a, b, c, p, u, v, w);
	return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
}

float3 NewellNormal(Polygon3D poly) {

	const vector<float3> v = poly.GetVertices();
	int n = v.size();
	float3 normal = make_float3(0.0f, 0.0f, 0.0f);
	for (int i = 0, j = n - 1; i < n; j = i, i++) {
		normal.x += (v[j].y - v[i].y) * (v[j].z + v[i].z); 
		normal.y += (v[j].z - v[i].z) * (v[j].x + v[i].x); 
		normal.z += (v[j].x - v[i].x) * (v[j].y + v[i].y); 
	}
	normalize(normal);
	return normal;

}