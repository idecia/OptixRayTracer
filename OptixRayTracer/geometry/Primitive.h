#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"

class Mesh3D;

class Primitive : public GeometricObject {

public:

	Primitive();
	bool IsPrimitive();
	virtual const Mesh3D* Triangulate() = 0;
	virtual ~Primitive();

};


Primitive::Primitive()
	:GeometricObject() {

}


bool Primitive::IsPrimitive() {

	return true;
}

Primitive::~Primitive() {

}