#pragma once

#include "core/optix_global.h"
#include "math/Matrix4x4.h"

class GeometricObject {

public:

	GeometricObject();
	void Translate(const float3& d);
	void Scale(const float3& s);
	void RotateX(const float r);
	void RotateY(const float r);
	void RotateZ(const float r);
	Matrix4x4 GetTransform();
	Matrix4x4 GetInverseTransform();
	virtual ~GeometricObject();

private:

	Matrix4x4 transf;
	Matrix4x4 invTransf;


};

GeometricObject::GeometricObject()
	: transf(Matrix4x4::identity()),
	  invTransf(Matrix4x4::identity()) {

}

Matrix4x4 GeometricObject::GetTransform() {

	return transf;

}


Matrix4x4 GeometricObject::GetInverseTransform() {

	return invTransf;

}

void GeometricObject::RotateX(const float theta) {

	Matrix4x4 m = RotationXMatrix(theta);
	Matrix4x4 invm = RotationXMatrix(-theta);
	transf = m * transf;
	invTransf = invm * transf;

}

void GeometricObject::RotateY(const float theta) {

	Matrix4x4 m = RotationYMatrix(theta);
	Matrix4x4 invm = RotationYMatrix(-theta);
	transf = m * transf;
	invTransf = invm * transf;

}

void GeometricObject::RotateZ(const float theta) {


	Matrix4x4 m = RotationZMatrix(theta);
	Matrix4x4 invM = RotationZMatrix(-theta);
	transf = m * transf;
	invTransf = invM * transf;

};

void GeometricObject::Scale(const float3& s) {

	Matrix4x4 M = ScalingMatrix(s);
	float3 invs = make_float3(1.0f / s.x, 1.0f / s.y, 1.0f / s.z);
	Matrix4x4 invM = ScalingMatrix(invs);
	transf = M * transf;
	invTransf = invM * transf;

}

void GeometricObject::Translate(const float3& d) {

	Matrix4x4 m = TranslationMatrix(d);
	Matrix4x4 invm = TranslationMatrix(-d);
	transf = m * transf;
	invTransf = invm * transf;

}