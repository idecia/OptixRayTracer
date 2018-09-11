#pragma once

#include "core/optix_global.h"
#include "geometry/GeometricObject.h"
#include <vector>

using namespace std;


class Compound : public GeometricObject {

public:

	Compound();
	const vector<GeometricObject*>& GetGeometricObjects();
	void AddGeometricObject(GeometricObject* obj);

private:

	vector<GeometricObject*> objects;

};

Compound::Compound() {

}

const vector<GeometricObject*>& Compound::GetGeometricObjects() {

	return objects;

}

void Compound::AddGeometricObject(GeometricObject* obj) {

	objects.push_back(obj);

}