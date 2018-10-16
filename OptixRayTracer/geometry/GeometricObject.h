#pragma once

#include "core/optix_global.h"
#include "math/Transform.h"
#include <vector>

using namespace std;

class GeometricObject {

public:

	GeometricObject();
	void Translate(const float3& d);
	void Scale(const float3& s);
	void RotateX(const float theta);
	void RotateY(const float theta);
	void RotateZ(const float theta);
	void SetObjectToWorldTransformation(const Transformation &T);
	Transformation GetObjectToWorldTransformation();
	virtual vector<GeometricObject*> GetChilds();
	virtual void AddChild(GeometricObject* obj);
	virtual bool IsPrimitive();
	virtual ~GeometricObject();

private:

	Transformation objectToWorld;

};

GeometricObject::GeometricObject()
	: objectToWorld() {

}

Transformation GeometricObject::GetObjectToWorldTransformation() {

	return objectToWorld;

}


void GeometricObject::SetObjectToWorldTransformation(const Transformation &T) {

	objectToWorld = T;

}

void GeometricObject::RotateX(const float theta) {

	Transformation Rx = RotationXTransformation(theta);
	objectToWorld = Rx * objectToWorld;
	
}

void GeometricObject::RotateY(const float theta) {

	Transformation Ry = RotationYTransformation(theta);
	objectToWorld = Ry * objectToWorld;

}

void GeometricObject::RotateZ(const float theta) {


	Transformation Rz = RotationZTransformation(theta);
	objectToWorld = Rz * objectToWorld;

};

void GeometricObject::Scale(const float3& s) {

	Transformation S = ScalingTransformation(s);
	objectToWorld = S * objectToWorld;

}

void GeometricObject::Translate(const float3& d) {

	Transformation T = TranslationTransformation(d);
	objectToWorld = T * objectToWorld;

}

vector<GeometricObject*> GeometricObject::GetChilds() {

	return vector<GeometricObject*>();
	//Throw Exception

}


void GeometricObject::AddChild(GeometricObject* obj) {

	//Throw Exception

}


bool GeometricObject::IsPrimitive() {

	return false;

}

