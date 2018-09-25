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
	void SetObjectToWorldTransform(const Transform &T);
	Transform GetObjectToWorldTransform();
	virtual vector<GeometricObject*> GetChilds();
	virtual void AddChild(GeometricObject* obj);
	virtual bool IsPrimitive();
	virtual ~GeometricObject();

private:

	Transform objectToWorld;

};

GeometricObject::GeometricObject()
	: objectToWorld() {

}

Transform GeometricObject::GetObjectToWorldTransform() {

	return objectToWorld;

}


void GeometricObject::SetObjectToWorldTransform(const Transform &T) {

	objectToWorld = T;

}

void GeometricObject::RotateX(const float theta) {

	Transform Rx = RotationXTransform(theta);
	objectToWorld = Rx * objectToWorld;
	
}

void GeometricObject::RotateY(const float theta) {

	Transformation Ry = RotationYTransform(theta);
	objectToWorld = Ry * objectToWorld;

}

void GeometricObject::RotateZ(const float theta) {


	Transform Rz = RotationZTransform(theta);
	objectToWorld = Rz * objectToWorld;

};

void GeometricObject::Scale(const float3& s) {

	Transform S = ScalingTransform(s);
	objectToWorld = S * objectToWorld;

}

void GeometricObject::Translate(const float3& d) {

	Transform T = TranslationTransform(d);
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

