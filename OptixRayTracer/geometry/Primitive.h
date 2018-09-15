#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"

class Mesh3D;

class Primitive : public GeometricObject {

public:

	Primitive();
	bool IsPrimitive();
	virtual Mesh3D* Triangulate();

}


Primitive::Primitive()
	:GeometricObject() {

}

bool Primitive::IsPrimitive() {

	return true;
}

Mesh3D* Primitive::Triangulate() {

	return 0;
}