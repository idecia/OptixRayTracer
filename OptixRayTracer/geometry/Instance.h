#pragma once

#include "core/optix_global.h"
#include "geometry/Primitive.h"

class Instance: public Primitive {

public:

	Instance(GeometricObject* obj);
	GeometricObject* GetGeometricObject();

private:

	GeometricObject* object;

};

Instance::Instance(GeometricObject* obj) 
	:Primitive(), 
	 object(obj) {
	
	
}


GeometricObject* Instance::GetGeometricObject() {

	return object;

}
