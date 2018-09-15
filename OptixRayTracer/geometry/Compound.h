#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"
#include <vector>

using namespace std;


class Compound : public GeometricObject {

public:

	Compound();
	const vector<GeometricObject*>& GetChilds();
	void AddChild(GeometricObject* obj);
	bool IsPrimitive();

private:

	vector<GeometricObject*> objects;

};

Compound::Compound() 
	: GeometricObject() {

}

const vector<GeometricObject*>& Compound::GetChilds() {

	return objects;

}

void Compound::AddChild(GeometricObject* obj) {

	objects.push_back(obj);

}

bool Compound::IsPrimitive() {

	return false;
}