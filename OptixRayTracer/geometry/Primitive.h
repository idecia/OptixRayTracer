#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"


class Primitive : public GeometricObject {

public:

	Primitive();
	bool IsPrimitive();
	virtual const Mesh3D* Tessellate() = 0;

};


Primitive::Primitive()
	:GeometricObject() {

}


bool Primitive::IsPrimitive() {

	return true;
}

