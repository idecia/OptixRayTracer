#pragma once

#include "core/optix_global.h"
#include "geometry/Mesh2D.h"
#include <vector>

using namespace std;

class Polygon2D {

private:

	vector<float2> vertices;

public:
	
	const vector<float2>& GetVertices() const;
	void AddVertex(const float2& v);
	Mesh2D Triangulate();


};

const vector<float2>& Polygon2D::GetVertices() const {

	return vertices;

}

void Polygon2D::AddVertex(const float2& v) {
	
	vertices.push_back(v);

}

Mesh2D Polygon2D::Triangulate() {


}
