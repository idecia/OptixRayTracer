#pragma once

#include "core/optix_global.h"
#include "geometry/Mesh2D.h"
#include "geometry/GeometryUtil.h"
#include <vector>

using namespace std;

class Polygon2D {

private:

	vector<float2> vertices;

public:
	
	Polygon2D();
	Polygon2D(const vector<float2> &vertices);
	const vector<float2>& GetVertices() const;
	void AddVertex(const float2& v);
	float PolygonArea();
	WindingType PolygonWinding();
	Mesh2D* Triangulate();


};

Polygon2D::Polygon2D() { }

Polygon2D::Polygon2D(const vector<float2> &vertices) {
	//TODO: Double Linked List instead of vector data structure?
}


const vector<float2>& Polygon2D::GetVertices() const {
	//TODO: Substitute by an iterator to encapsulate
	//internal data.
	return vertices;

}

void Polygon2D::AddVertex(const float2& v) {
	
	vertices.push_back(v);

}

float Polygon2D::PolygonArea() {

	const vector<float2> &v = vertices;
	int n = v.size();
	float area = 0.0f;
	for (int i = 0, j = n-1; i < n; j = i, i++) {
		area += (v[i].x + v[j].x) * (v[i].y - v[j].y);
	}
	return 0.5f * area;
}



WindingType Polygon2D::PolygonWinding() {
	//TODO: Return NONE when we have a degenerated polygon
	float area = PolygonArea();
	return (area > 0.0f) ? WindingType::CCW : WindingType::CW;

}


Mesh2D* Polygon2D::Triangulate() {

	int n = vertices.size();

	vector<int> prev(n), next(n);
	for (int i = 0; i < n; i++) {
		prev[i] = i - 1;
		next[i] = i + 1;	
	}
	prev[0]   = n - 1;
	next[n-1] = 0;

	Mesh2D* mesh = new Mesh2D();
	for (int z = 0; z < n; z++) {
		mesh->AddVertex(vertices[z]);
	}

	//TODO: Ver que hacer cuando no se encuentra oreja, por ejemplo,
	//en poligonos degenrados.
	WindingType PW = PolygonWinding();
	int i = 0;
	while (n > 3) {

		bool isEar = 1;
		const float2 &a = vertices[prev[i]];
		const float2 &b = vertices[i];
		const float2 &c = vertices[next[i]];
		if (TriangleWinding(a, b, c) == PW) {
			int k = next[next[i]];
			do {
				if (TestPointTriangle(a, b, c, vertices[k])) {
					isEar = 0;
					break;
				}
				k = next[k];
			} while (k != prev[i]);
		}
		else {
			isEar = 0;
		}

		if (isEar) {
			Face* f = new Face(prev[i], i, next[i]);
			mesh->AddFace(f);
			next[prev[i]] = next[i];
			prev[next[i]] = prev[i];
			i = prev[i];
			n--;
		}
		else {
			i = next[i];
		}

	}

	if (n == 3) {
		Face* f = new Face(prev[i], i, next[i]);
		mesh->AddFace(f);
	}

	return mesh;

}

