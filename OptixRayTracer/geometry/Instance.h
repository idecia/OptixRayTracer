#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"

class Instance: public GeometricObject {

public:

	Instance(GeometricObject* obj);
    vector<GeometricObject*> GetChilds();
    void AddChild(GeometricObject* obj);

private:

	GeometricObject* object;

};

Instance::Instance(GeometricObject* obj) 
	:GeometricObject(),
	 object(obj) {
	
	
}


vector<GeometricObject*> Instance::GetChilds() {

	vector<GeometricObject*> objects;
	objects.push_back(object);
	return objects;

}

void Instance::AddChild(GeometricObject* obj) {
	
	object = obj;

}
