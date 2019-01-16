#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"
#include "geometry/Compound.h"
#include "geometry/Instance.h"
#include "geometry/Polygon2D.h"
#include "geometry/Mesh3D.h"
#include "geometry/Polygon3D.h"


class RectangularBlind {

private:

	float width;
	float height;
	float longitude;
	float angle;
	float pad;
	int nSlats;

public:

	RectangularBlind(float w, float h, float l, float a, float p, int n);
	void SetWidth(float w);
	void SetHeight(float h);
	void SetLongitude(float l);
	void SetAngle(float a);
	void SetPad(float p);
	void SetNumSlats(int n);
	 Mesh3D* GetMesh();


};


void GetWorldMesh(GeometricObject* obj, Transformation objToWorld, int &vertexOffset, Mesh3D* mesh) {

	if (obj->IsPrimitive()) {
		const Mesh3D* mesh3D = ((Primitive*)obj)->Triangulate();
		const vector<float3>& vertices = mesh3D->GetVertices();
		objToWorld = objToWorld * mesh3D->GetObjectToWorldTransformation();
		for (int i = 0; i < vertices.size(); i++) {
			const float3 v = objToWorld * vertices[i];
			mesh->AddVertex(v);
		}
		const vector<Face*>& faces = mesh3D->GetFaces();
		for (int i = 0; i < faces.size(); i++) {
			const vector<int>& indices = faces[i]->GetIndices();
			Face* face = new Face();
			for (int j = 0; j < indices.size(); j++) {
				face->AddIndex(indices[j] + vertexOffset);
			}
			mesh->AddFace(face);
		}
		vertexOffset += vertices.size();
	}
	else {
		vector<GeometricObject*> childs = obj->GetChilds();
		Transformation objToWorldChild = obj->GetObjectToWorldTransformation();
		for (int i = 0; i < childs.size(); i++) {
			GeometricObject* child = childs[i];
			GetWorldMesh(child, objToWorld * objToWorldChild, vertexOffset, mesh);
		}
	}

}

Mesh3D* GetWorldMesh(GeometricObject* obj) {

	Transformation identity;
	int vertexOffset = 0;
	Mesh3D* mesh = new Mesh3D();
	GetWorldMesh(obj, identity, vertexOffset, mesh);
	return mesh;


}

RectangularBlind::RectangularBlind(float w, float h, float l, float a, float p, int n)
	: width(w), height(h), longitude(l), angle(a), pad(p), nSlats(n)  {

}

void RectangularBlind::SetWidth(float w) {
	width = w;
}

void RectangularBlind::SetHeight(float h) {
	height = h;
}

void RectangularBlind::SetLongitude(float l) {
	longitude = l;
}

void RectangularBlind::SetAngle(float a) {
	angle = a;
}

void RectangularBlind::SetPad(float p) {
	pad = p;
}

void RectangularBlind::SetNumSlats(int n) {
	nSlats = n;
}


 Mesh3D*  RectangularBlind::GetMesh() {


	//float w = 0.01,h = 0.2 ,l = 3;
	Polygon2D poly;
	poly.AddVertex(make_float2(0.0f, 0.0f));
	poly.AddVertex(make_float2(0.0f, height));
	poly.AddVertex(make_float2(width, height));
	poly.AddVertex(make_float2(width, 0.0f));
	Mesh3D* slat = Extrude(&poly, make_float3(0.0f, 0.0f, longitude));
	//slat->Triangulate();
	slat->RotateY(-M_PIf / 2.0f);
	slat->RotateX(angle);
	
	GeometricObject* blind = new Compound();
	vector<Instance*> instances;
	for (int i = 0; i < nSlats; i++) {
		Instance* inst = new Instance(slat);
		inst->Translate(make_float3(1.0f, -0.25f, 1.0f));
		inst->Translate(make_float3(0.0f, 0.0f, i*(width + pad)));  
		blind->AddChild(inst);
		instances.push_back(inst);
	}

	Mesh3D* mesh =  GetWorldMesh(blind);

	delete slat;
	delete blind;
	for (int i = 0; i < instances.size(); i++) {
		delete instances[i];
	}

	return mesh;


} 

