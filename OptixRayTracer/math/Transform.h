#pragma once

#include "core/optix_global.h"
#include "math/Matrix4x4.h"

class Transformation {

public:

	Transformation();
	Transformation(const Matrix4x4 &M, const Matrix4x4 &invM);
	Transformation(const Transformation &T);
	Transformation& operator=(const Transformation &T);
	Transformation operator*(const Transformation &T);
	float3 operator*(const float3 &v);

private:

	Matrix4x4 M;
	Matrix4x4 invM;

};

Transformation::Transformation()
	: M(Matrix4x4::identity()),
 	  invM(Matrix4x4::identity()) {


}

Transformation::Transformation(const Matrix4x4 &M, const Matrix4x4 &invM)
	:M(M), invM(invM) {

}

Transformation::Transformation(const Transformation &T)
	:M(T.M), invM(T.invM) {
	

}

Transformation& Transformation::operator=(const Transformation &T) {

	M = T.M;
	invM = T.invM;
	return *(this);

}

Transformation Transformation::operator*(const Transformation &T) {
		
	return Transformation(M*T.M, T.invM*invM);

}

float3 Transformation::operator*(const float3 &v) {

	/*float x = M[0][0] * v.x + M[0][1] * v.y + M[0][2] * v.z + M[0][3];
	float y = M[1][0] * v.x + M[1][1] * v.y + M[1][2] * v.z + M[1][3];
	float z = M[2][0] * v.x + M[2][1] * v.y + M[2][2] * v.z + M[2][3];
	float w = M[3][0] * v.x + M[3][1] * v.y + M[3][2] * v.z + M[3][3];
	return make_float3(x / w, y / w, z / w);*/
	
}

Transformation RotationXTransformation(float theta) {
	Matrix4x4 M = RotationXMatrix(theta);
	Matrix4x4 invM = RotationXMatrix(-theta);
	return Transformation(M, invM);
}

Transformation RotationYTransformation(float theta) {

	Matrix4x4 M = RotationYMatrix(theta);
	Matrix4x4 invM = RotationYMatrix(-theta);
	return Transformation(M, invM);
}

Transformation RotationZTransformation (float theta) {

	Matrix4x4 M = RotationZMatrix(theta);
	Matrix4x4 invM = RotationZMatrix(-theta);
	return Transformation(M, invM);
}


Transformation ScalingTransformation(float3 s) {

	Matrix4x4 M = ScalingMatrix(s); 
	Matrix4x4 invM = ScalingMatrix(make_float3(1.0f/s.x, 1.0f/s.y, 1.0f/s.z));
	return Transformation(M, invM);
}

Transformation TranslationTransformation(const float3& d) {

	Matrix4x4 M = TranslationMatrix(d);
	Matrix4x4 invM = TranslationMatrix(-d);
	return Transformation(M, invM);

}