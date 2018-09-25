#pragma once

#include "core/optix_global.h"
#include "math/Matrix4x4.h"

class Transform {

	Transform();
	Transform(Matrix4x4 M, Matrix4x4 invM);
	Transform(const Transform & T);
	Transform& operator=(const Transform& T);
	Transform operator*(const Transform& T);

private:

	Matrix4x4 M;
	Matrix4x4 invM;

}

Transform::Transform()
	: M(Matrix4x4::identity()),
 	  invM(Matrix4x4::identity()) {


}

Transform::Transform(Matrix4x4 M, Matrix4x4 invM)
	:M(M), invM(invM) {

}

Transform::Transform(const Transform & T)
	:M(T.M), invM(T.invM) {
	

}

Transform& Transform::operator=(const Transform& T) {

	M = T.M;
	invM = T.invM;
	return *(this);

}

Transform Transform::operator*(const Transform& T) {
		
	return Transform(M*T.M, T.invM*invM);

}


Transform RotationXTransform(float theta) {

	Matrix4x4 M = RotationXMatrix(theta);
	Matrix4x4 invM = RotationXMatrix(-theta);
	return Transform(M, invM);
}

Transform RotationYTransform(float theta) {

	Matrix4x4 M = RotationYMatrix(theta);
	Matrix4x4 invM = RotationYMatrix(-theta);
	return Transform(M, invM);
}

Transform RotationZTransform(float theta) {

	Matrix4x4 M = RotationZMatrix(theta);
	Matrix4x4 invM = RotationZMatrix(-theta);
	return Transform(M, invM);
}


Transform ScalingTransform(float3 s) {

	Matrix4x4 M = ScalingMatrix(s);
	Matrix4x4 invM = ScalingMatrix(make_float3(1.0f/s.x, 1.0f/s.y, 1.0f/s.z));
	return Transform(M, invM);
}

void TranslationTransform(const float3& d) {

	Matrix4x4 M = TranslationMatrix(d);
	Matrix4x4 invM = TranslationMatrix(-d);
	return Transform(M, invM);

}