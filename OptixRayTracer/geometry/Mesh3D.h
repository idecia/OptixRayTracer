#pragma once

#include "core/optix_global.h"
#include "geometry/Face.h"
#include "geometry/Polygon3D.h"
#include "geometry/Polygon2D.h"
#include "geometry/Primitive.h"
#include "collections/vector.h"
#include <vector>

using namespace std;


class Mesh3D : public Primitive {

private:

	vector<float3> vertices;
	vector<Face*> faces;
	bool isTesselated;

	Polygon3D* FaceToPoly3D(const Face* face);
	Polygon2D* ProjectFace(const Face* face);
	vector<Face*> TriangulateFace(int faceIdx);
	

public:

	Mesh3D();
	const vector<float3>& GetVertices() const;
	const vector<Face*>& GetFaces() const;
	void AddFace(Face* f);
	void AddVertex(const float3& v);
	const Mesh3D* Triangulate();
	virtual ~Mesh3D();

};

Mesh3D::Mesh3D()
	:Primitive() {

}

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

Mesh3D* Extrude(const Polygon2D* poly, const float3 &direction) {

	Mesh3D* mesh = new Mesh3D();
	const vector<float2> &vertices = poly->GetVertices();
	int n = vertices.size();

	Face* bottom = new Face();
	for (int i = 0; i < n; i++) {
		const float2 &v2 = vertices[i];
		float3 v3 = make_float3(v2.x, v2.y, 0.0f);
		mesh->AddVertex(v3);
		bottom->AddIndex(i);
	}
	mesh->AddFace(bottom);

	Face* top = new Face();
	for (int i = 0; i < n; i++) {
		const float2 &v2 = vertices[i];
		float3 v3 = make_float3(v2.x, v2.y, 0.0f);
		mesh->AddVertex(v3 + direction);
		top->AddIndex(i + n);

	}
	mesh->AddFace(top);

	for (int i = 0; i < n; i++) {
		Face* side = new Face(i, (i + 1)%n, (i + 1)%n + n, i + n);
		mesh->AddFace(side);
	}

	return mesh;

}

Polygon3D* Mesh3D::FaceToPoly3D(const Face* face) {

	Polygon3D* poly3D = new Polygon3D();
	const vector<int> &ind = face->GetIndices();
	int n = ind.size();
	for (int i = 0; i < n; i++) {
		poly3D->AddVertex(vertices[ind[i]]);
	}
	return poly3D;
}

Polygon2D* Mesh3D::ProjectFace(const Face* face) {

	Polygon3D* poly3D = FaceToPoly3D(face);
	Polygon2D* poly2D = ProjectPoly3D(poly3D);
	delete poly3D;
	return poly2D;

}

vector<Face*> Mesh3D::TriangulateFace(int faceIdx) {

	const Face* face = faces[faceIdx];
	const vector<int> &ind = face->GetIndices();
	Polygon2D* poly2D = ProjectFace(face);

	vector<Face*> triFaces;
	
	//ear cutting
	const vector<float2> &vertices = poly2D->GetVertices();
	int n = vertices.size();
	vector<int> prev(n), next(n);
	for (int i = 0; i < n; i++) {
		prev[i] = i - 1;
		next[i] = i + 1;
	}
	prev[0] = n - 1;
	next[n - 1] = 0;

	WindingType PW = poly2D->PolygonWinding();
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
			Face* f = new Face(ind[prev[i]], ind[i], ind[next[i]]);
			triFaces.push_back(f);
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
		Face* f = new Face(ind[prev[i]], ind[i], ind[next[i]]);
		triFaces.push_back(f);
	}

	return triFaces;

}

const Mesh3D* Mesh3D::Triangulate() {
	
	if (!isTesselated) {
		vector<Face*> triFaces;
		int nFaces = faces.size();
		for (int i = 0; i < nFaces; i++) {
			vector<Face*> newFaces = TriangulateFace(i);
			Append(triFaces, newFaces);
		}
		faces.clear();
		faces = triFaces;
		isTesselated = true;
	}
	return this;

}