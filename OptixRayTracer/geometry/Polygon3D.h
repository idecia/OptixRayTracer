#pragma once

#include "core/optix_global.h"
#include <vector>

using namespace std;

class Polygon3D {

private:

	vector<float3> vertices;

public:

	const vector<float3>& GetVertices() const;
	void AddVertex(const float3& v);
	float3 NewellNormal();


};

const vector<float3>& Polygon3D::GetVertices() const {

	return vertices;

}

void Polygon3D::AddVertex(const float3& v) {

	vertices.push_back(v);

}

float3 Polygon3D::NewellNormal() {

	const vector<float3> &v = vertices;
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