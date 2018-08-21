#pragma once

#include "core/optix_global.h"
#include "geometry/Face.h"
#include <vector>

using namespace std;

class Mesh3D {

private:

	vector<float3> vertices;
	vector<Face*> faces;

public:

	const vector<float3>& GetVertices() const;
	const vector<Face*>& GetFaces() const;
	void AddFace(Face* f);
	void AddVertex(const float3& v);
	void Triangulate();
	virtual ~Mesh3D();

};

const vector<float3>& Mesh3D::GetVertices() const {

	return vertices;
}

const vector<Face*>& Mesh3D::GetFaces() const {

	return faces;

}

void Mesh3D::AddFace(Face* f) {

	faces.push_back(f);
}

void Mesh3D::AddVertex(const float3& v) {

	vertices.push_back(v);

}

Mesh3D::~Mesh3D() {

	vector<Face*>::iterator it;
	for (it = faces.begin(); it != faces.end(); it++) {
		delete (*it);
	}
}

void Mesh3D::Triangulate() {

	/*for (int i = 0; i < faces.size(); i++) {
		
		Face* oldFace = faces[i];
		vector<int> ind = oldFace->GetIndices();
		Polygon3D* poly = new Polygon2D();
		for (int j = 0; j < ind.size(); j++){
			poly.AddVertex(vertices[ind[j]]);
		}
		float3 normal = poly.NewellNormal();




		Face* newFace = new Face();*/






	
	


}