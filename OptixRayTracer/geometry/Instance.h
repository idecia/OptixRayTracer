#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"
#include "math/Matrix4x4.h"

class Instance: public GeometricObject {

public:

	Instance(GeometricObject* obj);
	GeometricObject* GetGeometricObject();

private:

	GeometricObject* obj;

};

Instance::Instance(GeometricObject* obj) 
	:GeometricObject(), obj(obj) {
	
	
}

GeometricObject* Instance::GetGeometricObject() {

	return obj;

}
