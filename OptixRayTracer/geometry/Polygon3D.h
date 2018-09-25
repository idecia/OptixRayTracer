#pragma once

#include "core/optix_global.h"
#include "geometry/Polygon2D.h"
#include <vector>

using namespace std;

class Polygon3D {

private:

	vector<float3> vertices;

public:

	const vector<float3>& GetVertices() const;
	void AddVertex(const float3& v);


};

const vector<float3>& Polygon3D::GetVertices() const {

	return vertices;

}

void Polygon3D::AddVertex(const float3& v) {

	vertices.push_back(v);

}

float3 NewellNormal(Polygon3D* poly3D) {

	const vector<float3> &v = poly3D->GetVertices();
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

Polygon2D* ProjectPoly3D(Polygon3D* poly3D) {

	float3 normal = NewellNormal(poly3D);
	int u, v;
	GetProjectionAxes(normal, u, v);
	Polygon2D* poly2D = new Polygon2D();
	const vector<float3> &vertices = poly3D->GetVertices();
	for (int i = 0; i < vertices.size(); i++) {
		float3 vec = vertices[i];
		poly2D->AddVertex(ProjectVec3DTo2D(vec, u, v));
	}
	return poly2D;
}
