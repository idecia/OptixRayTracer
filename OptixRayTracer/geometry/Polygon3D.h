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


};

const vector<float3>& Polygon3D::GetVertices() const {

	return vertices;

}

void Polygon3D::AddVertex(const float3& v) {

	vertices.push_back(v);

}
