#pragma once

#include "core/optix_global.h"
#include "geometry/Face.h"
#include "geometry/Polygon3D.h"
#include "geometry/Polygon2D.h"
#include "geometry/Primitive.h"
#include <vector>

using namespace std;

class Mesh3D : public Primitive {

private:

	vector<float3> vertices;
	vector<Face*> faces;

	Polygon3D* FaceToPoly3D(const Face* &f);
	Polygon2D* ProjectFace(const Face* &f);
	vector<Face*> TriangulatePoly2D(Polygon2D* poly2D, const vector<int> &ind);

public:

	Mesh3D();
	const vector<float3>& GetVertices() const;
	const vector<Face*>& GetFaces() const;
	void AddFace(Face* f);
	void AddVertex(const float3& v);
	void Triangulate();
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


void Append(vector<Face*> &vec1, vector<Face*> &vec2) {

	int n = vec2.size();
	for (int i = 0; i < n; i++) {
		vec1.push_back(vec2[i]);
	}
}

float ValueAt(float3 v, int i) {
	if (i == 0)
		return v.x;
	if (i == 1)
		return v.y;
	return v.z;
}


void GetProjectionAxes(const float3 &normal, int &u, int &v) {

	u = 0; v = 1;
	if (abs(normal.y) > abs(normal.x) &&
		abs(normal.y) > abs(normal.z)) {
		u = 0;
		v = 2;
	}
	else if (abs(normal.x) > abs(normal.y) &&
		abs(normal.x) > abs(normal.z)) {
		u = 1;
		v = 2;
	}

}

float2 ProjectVec3DTo2D(float3 vec, int u, int v) {

	return make_float2(ValueAt(vec, u), ValueAt(vec, v));

}

Polygon2D* ProjectPoly3D(Polygon3D* poly3D) {

	float3 normal = poly3D->NewellNormal();
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

Polygon3D* Mesh3D::FaceToPoly3D(const Face* &f) {

	Polygon3D* poly3D = new Polygon3D();
	const vector<int> &ind = f->GetIndices();
	int n = ind.size();
	for (int i = 0; i < n; i++) {
		poly3D->AddVertex(vertices[ind[i]]);
	}
	return poly3D;
}

Polygon2D* Mesh3D::ProjectFace(const Face* &f) {

	Polygon3D* poly3D = FaceToPoly3D(f);
	Polygon2D* poly2D = ProjectPoly3D(poly3D);
	delete poly3D;
	return poly2D;

}

vector<Face*> Mesh3D::TriangulatePoly2D(Polygon2D* poly2D, const vector<int> &ind) {

	vector<Face*> faces;
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
			faces.push_back(f);
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
		faces.push_back(f);
	}
	return faces;

}

void Mesh3D::Triangulate() {

	vector<Face*> triFaces;
	int nFaces = faces.size();
	for (int i = 0; i < nFaces; i++) {
		const Face* face = faces[i];
		const vector<int> &ind = face->GetIndices();
		Polygon2D* poly2D = ProjectFace(face);
		vector<Face*> newFaces = TriangulatePoly2D(poly2D, ind);
		delete poly2D;
		Append(triFaces, newFaces);
	}
	faces.clear();
	faces = triFaces;

}

Mesh3D* Extrude(const Polygon2D* poly, const float3 &direction) {

	Mesh3D* mesh = new Mesh3D();
	const vector<float2> &vertices = poly->GetVertices();
	int n = vertices.size();
	Face* bottom = new Face();
	for (int i = 0; i < n; i++) {
		const float2 &v = vertices[i];
		mesh->AddVertex(make_float3(v.x, v.y, 0.0f));
		bottom->AddIndex(i);
	}
	mesh->AddFace(bottom);
	Face* top = new Face();
	for (int i = 0; i < n; i++) {
		const float2 &v = vertices[i];
		mesh->AddVertex(make_float3(v.x, v.y, 0.0f) + direction);
		top->AddIndex(i + n);
	}
	mesh->AddFace(top);
	for (int i = 0; i < n; i++) {
		Face* lateral = new Face(i, (i + 1)%n, (i + 1)%n + n, i + n);
		mesh->AddFace(lateral);
	}
	return mesh;

}