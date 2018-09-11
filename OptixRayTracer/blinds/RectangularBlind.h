#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"
#include "geometry/Compound.h"
#include "geometry/Instance.h"
#include "geometry/Polygon2D.h"
#include "geometry/Mesh3D.h"
#include "geometry/Polygon3D.h"

class RectangularBlind {

	GeometricObject* BuildRectangularBlind();

};

GeometricObject* RectangularBlind::BuildRectangularBlind() {

	float w = 2,h = 3 ,l = 10;
	Polygon2D poly;
	poly.AddVertex(make_float2(0.0f, 0.0f));
	poly.AddVertex(make_float2(w, 0.0f));
	poly.AddVertex(make_float2(w, h));
	poly.AddVertex(make_float2(0.0f, h));
	Mesh3D* slat = Extrude(&poly, make_float3(0.0f, 0.0f, l));
	slat->Translate(make_float3(-w / 2.0f, -h / 2.0f, -l / 2.0f));
	slat->RotateX(M_PIf / 2.0f);
	
	Compound* blind = new Compound();
	float pad = 0.5;
	for (int i = 0; i < 5; i++) {
		Instance* inst = new Instance(slat);
		inst->Translate(make_float3(0.0f, 0.0f, i*(h + pad)));
		blind->AddGeometricObject(inst);
	}

	return blind;


}