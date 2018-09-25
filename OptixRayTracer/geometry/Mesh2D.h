#pragma once

#include "core/optix_global.h"
#include "geometry/Face.h"
#include <vector>

using namespace std;

class Mesh2D {

private:

	vector<float2> vertices;
	vector<Face*> faces;

public:

	const vector<float2>& GetVertices() const;
	const vector<Face*>& GetFaces() const;
	void AddFace(Face* f);
	void AddVertex(const float2& v);
	virtual ~Mesh2D();

};

const vector<float2>& Mesh2D::GetVertices() const {

	return vertices;
}

const vector<Face*>& Mesh2D::GetFaces() const {

	return faces; 
	
}

void Mesh2D::AddFace(Face* f) {

	faces.push_back(f);
}

void Mesh2D::AddVertex(const float2& v) {

	vertices.push_back(v);

}

Mesh2D::~Mesh2D() {

	vector<Face*>::iterator it;
	for (it = faces.begin(); it != faces.end(); it++) {
		delete (*it);
	}
}

