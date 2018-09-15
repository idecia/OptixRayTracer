#pragma once

#include "core/optix_global.h"
#include "math/Matrix4x4.h"

class GeometricObject {

public:

	GeometricObject();
	void Translate(const float3& d);
	void Scale(const float3& s);
	void RotateX(const float theta);
	void RotateY(const float theta);
	void RotateZ(const float theta);
	Matrix4x4 GetTransformation();
	Matrix4x4 GetInverseTransformation();
	virtual bool IsPrimitive();
	virtual ~GeometricObject();

private:

	Matrix4x4 transformation;
	Matrix4x4 invTransformation;

	void UpdateTransformation(const Matrix4x4 &m, const Matrix4x4 &invm);


};

GeometricObject::GeometricObject()
	: transformation(Matrix4x4::identity()),
	  invTransformation(Matrix4x4::identity()) {

}

Matrix4x4 GeometricObject::GetTransformation() {

	return transformation;

}


Matrix4x4 GeometricObject::GetInverseTransformation() {

	return invTransformation;

}

void GeometricObject::UpdateTransformation(const Matrix4x4 &m, const Matrix4x4 &invm) {

	transformation = m * transformation;
	invTransformation = invTransformation * invm;
}

void GeometricObject::RotateX(const float theta) {

	Matrix4x4 m = RotationXMatrix(theta);
	Matrix4x4 invm = RotationXMatrix(-theta);
	UpdateTransformation(m, invm);
	
}

void GeometricObject::RotateY(const float theta) {

	Matrix4x4 m = RotationYMatrix(theta);
	Matrix4x4 invm = RotationYMatrix(-theta);
	UpdateTransformation(m, invm);

}

void GeometricObject::RotateZ(const float theta) {


	Matrix4x4 m = RotationZMatrix(theta);
	Matrix4x4 invm = RotationZMatrix(-theta);
	UpdateTransformation(m, invm);

};

void GeometricObject::Scale(const float3& s) {

	Matrix4x4 m = ScalingMatrix(s);
	float3 invs = make_float3(1.0f/s.x, 1.0f/s.y, 1.0f/s.z);
	Matrix4x4 invm = ScalingMatrix(invs);
	UpdateTransformation(m, invm);

}

void GeometricObject::Translate(const float3& d) {

	Matrix4x4 m = TranslationMatrix(d);
	Matrix4x4 invm = TranslationMatrix(-d);
	UpdateTransformation(m, invm);

}

bool GeometricObject::IsPrimitive() {

	return false;

}