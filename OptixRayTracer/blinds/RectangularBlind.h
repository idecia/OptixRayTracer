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



public:

	RectangularBlind(float w, float t, float l, float a, float s, int N, float H, float W, float L);
	RectangularBlind(float w, float t, float l, float a, float s, int N, float c, float H, float W, float L);
	void RepairBlind();
	 Mesh3D* GetMesh();
	 float w;
	 float t;
	 float l;
	 float a;
	 float s;
	 int N;
	 float c;

	 float  H;
	 float W;
	 float L;

	 float Rext;

	 static float wmin; 
	 static float wmax;
	 static float tmin ;
	 static float tmax;
	 static float lmin;
	 static float lmax ;
	 static float amin ;
	 static float amax;
	 static float smin ;
	 static float smax ;
	 static float cmin ;
	 static float cmax;
	 static int NMax;



};


 float RectangularBlind::wmin = 0.1f;
  float RectangularBlind::wmax = 1.0f;
  float RectangularBlind::tmin = 0.005;
  float RectangularBlind::tmax = 0.01;
  float RectangularBlind::lmin = 1.0f;
  float RectangularBlind::lmax = 2.0f;
  float RectangularBlind::amin = 0.0f;
  float RectangularBlind::amax = M_PIf / 2.0f;
  float RectangularBlind::smin = 0.0f;
  float RectangularBlind::smax = 1.0f;
  float RectangularBlind::cmin = 0.0f;
  float RectangularBlind::cmax = wmax / 2.0f;
  int RectangularBlind::NMax = 10;


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


RectangularBlind::RectangularBlind(float w, float t, float l, float a, float s, int N, float c, float H, float W, float L)
	: w(w), t(t), l(l), a(a), s(s), N(N), c(c), H(H), W(W), L(L) {

	if (c > 0) 
		Rext = ((4 * c*c) + (w*w)) / (8 * c);


}


/*
 Mesh3D*  RectangularBlind::GetMesh() {



	//float w = 0.01,h = 0.2 ,l = 3;
	Polygon2D poly;
	poly.AddVertex(make_float2(0.0f, 0.0f));
	poly.AddVertex(make_float2(0.0f, t));
	poly.AddVertex(make_float2(w, t));
	poly.AddVertex(make_float2(w, 0.0f));
	Mesh3D* slat = Extrude(&poly, make_float3(0.0f, 0.0f, l));
	//slat->Triangulate();
	slat->RotateY(-M_PIf / 2.0f);
	slat->RotateX(a);
	
	GeometricObject* blind = new Compound();
	vector<Instance*> instances;
	for (int i = 0; i < N; i++) {
	//	if (i*(w + s) <= (1.5 - w)) {
			Instance* inst = new Instance(slat);
			inst->Translate(make_float3(1.0f, -0.25f, 1.0f));
			inst->Translate(make_float3(0.0f, 0.0f, i*(w + s)));
			blind->AddChild(inst);
			instances.push_back(inst);
		//}
	}

	Mesh3D* mesh =  GetWorldMesh(blind);

	delete slat;
	delete blind;
	for (int i = 0; i < instances.size(); i++) {
		delete instances[i];
	}

	return mesh;


} 
*/

void RectangularBlind::RepairBlind() {

	w = min(w, wmax);
	w = max(w, wmin);

	l = min(l, lmax);
	l = max(l, lmin);

	a = min(a, amax);
	a = max(a, amin);

	s = min(s, smax);
	s = max(s, smin);

	c = min(c, cmax);
	c = max(c, cmin);

	if (2 * c > w) {
		c = w / 2.0f;
		Rext = ((4 * c*c) + (w*w)) / (8 * c);
	}

	if ((c > 0) && s < w) {
		float aux = acos(s / (2.0f*Rext)) - acos(w / (2.0f*Rext));
		if (a < aux)
			a = aux;
	}

	float h = 1.0f;
	float aux;
	if ((c > 0) && (sinf(a) < w / (2 * Rext)))
		aux = L / (Rext*(1 - cosf(a)) + c*cosf(a) + (w / 2.0f)*sinf(a));
	else
		aux = L / (w*sinf(a));

	if (aux < 1.0f)
		h = aux;

	w = h*w;
	c = h*c;
	Rext = h*Rext;

	h = 1.0f;
	if ((c > 0) && (cosf(a) < w / (2 * Rext)))
		aux = H / (Rext*(1 - sinf(a)) + c*sinf(a) + (w / 2.0f)*cosf(a));
	else
		aux = H / (w*cosf(a));

	if (aux < 1.0f)
		h = aux;

	w = h*w;
	c = h*c;
	Rext = h*Rext;

	N =  (int)floorf((H - (H/aux))/s + 1);
	N = min(N, NMax);

}

Mesh3D*  RectangularBlind::GetMesh() {

	Polygon2D poly;
	float wOver2 = w / 2.0f;
	float sinAlpha = sinf(a);
	float cosAlpha = cosf(a);
	float LBlind = w*sinAlpha;
	float HBlind = w*cosAlpha;
	if (c > 0) {
		float thetaMin = acos(w / (2 * Rext));
		float thetaMax = M_PIf - thetaMin;
		float nArcs = 5;
		float thetastep = (thetaMax - thetaMin) / nArcs;
		for (int i = 0; i <= nArcs; i++) {
			float theta = thetaMax - i*thetastep;
			float x = Rext*cos(theta);
			float y = Rext*sin(theta);
			poly.AddVertex(make_float2(x, y));
		}
		float Rint = Rext - t;
		for (int i = 0; i <= nArcs; i++) {
			float theta = thetaMin + i*thetastep;
			float x = Rint*cos(theta);
			float y = Rint*sin(theta);
			poly.AddVertex(make_float2(x, y));
		}
		if (sinAlpha < w/(2 * Rext)) {
			LBlind = Rext*(1 - cosAlpha) + c*cosAlpha + wOver2*sinAlpha;
		}
		if (cosAlpha < w / (2 * Rext)) {
			HBlind = Rext*(1 - sinAlpha) + c*sinAlpha + wOver2*cosAlpha;
		}
	}
	else {
		//float w = 0.01,h = 0.2 ,l = 3;
		poly.AddVertex(make_float2(-wOver2, 0.0f));
		poly.AddVertex(make_float2(-wOver2, t));
		poly.AddVertex(make_float2(wOver2, t));
		poly.AddVertex(make_float2(wOver2, 0.0f));
	}

	//a = acos(s/(2*Rext)) - acos(w/(2*Rext));

	Mesh3D* slat = Extrude(&poly, make_float3(0.0f, 0.0f, l));
	if (c > 0)
		slat->Translate(make_float3(0.0f, -Rext, -l/2.0f));
	else
		slat->Translate(make_float3(0.0f, -t, -l/2.0f));

	//slat->Triangulate();
	slat->RotateY(-M_PIf / 2.0f);
	slat->RotateX(a);
	
	float shiftH = c*sinAlpha + wOver2*cosAlpha;
	float shiftL = -L + c*cosAlpha + wOver2*sinAlpha;
	float padH = (H - HBlind - s*(N - 1)) / 2.0f;
	float padL = (L - LBlind) / 2.0f;
	slat->Translate(make_float3(0.0f, shiftL +padL, padH + shiftH));

	GeometricObject* blind = new Compound();
	vector<Instance*> instances;
	for (int i = 0; i < N; i++) {
		//	if (i*(w + s) <= (1.5 - w)) {
		Instance* inst = new Instance(slat);
		inst->Translate(make_float3(0.0f, 0.0f, 1.0f + i*s));
		blind->AddChild(inst);
		instances.push_back(inst);
		//}
	}

	Mesh3D* mesh = GetWorldMesh(blind);

	delete slat;
	delete blind;
	for (int i = 0; i < instances.size(); i++) {
		delete instances[i];
	}

	return mesh;


}