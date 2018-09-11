#pragma once

#include "core/optix_global.h"
/*
class Matrix4x4 {

public:

	Matrix4x4();
	Matrix4x4(const Matrix4x4& mat);
	Matrix4x4& operator=(const Matrix4x4& mat);
	Matrix4x4 operator*(const Matrix4x4& mat) const;
	Matrix4x4 operator/(const float d) const;
	float4 operator*(const float4& v) const;

private:

	float m[4][4];


};

Matrix4x4::Matrix4x4() {
	
	for(int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			if (i == j)
				m[i][j] = 1.0f;
			else
				m[i][j] = 0.0f;
		}
}

Matrix4x4::Matrix4x4(const Matrix4x4& mat) {

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = mat.m[i][j];
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& mat) {

	if (this == &mat)
		return (*this);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = mat.m[i][j];

	return (*this);

}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const {

	Matrix4x4 prod;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			float sum = 0.0f;
			for (int k = 0; k < 4; k++) 
				sum += m[i][k] * mat.m[k][j];
			prod.m[i][j] = sum;
		}
	return prod;
}


Matrix4x4 Matrix4x4:: operator/(const float d) const {

	Matrix4x4 div;
	float invd = 1.0f / d;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			div.m[i][j] = invd * m[i][j] ;
		}
	return div;
}

float4 Matrix4x4::operator*(const float4& v) const {

	float res[4];
	for (int i = 0; i < 4; i++)
		res[i] = m[i][1]*v.x + m[i][2]*v.y + m[i][3]*v.z + m[i][4]*v.w;
	return make_float4(res[1], res[2], res[3], res[4]);



}
*/

Matrix4x4 RotatationXMatrix(float theta) {

	Matrix4x4 m = Matrix4x4::identity();
	float costheta = cos(theta);
	float sintheta = sin(theta);
	m[0*4 + 1] = costheta;
	m[1*4 + 2] = -sintheta;	
	m[2*4 + 1] = sintheta;
	m[2*4 + 2] = costheta;
	return m;

}

Matrix4x4 RotatationYMatrix(float theta) {

	Matrix4x4 m = Matrix4x4::identity();
	float costheta = cos(theta);
	float sintheta = sin(theta);
	m[0*4 + 1] = costheta;
	m[0*4 + 2] = sintheta;
	m[2*4 + 0] = -sintheta;
	m[2*4 + 2] = costheta;
	return m;

}

Matrix4x4 RotatationZMatrix(float theta) {

	Matrix4x4 m = Matrix4x4::identity();
	float costheta = cos(theta);
	float sintheta = sin(theta);
	m[0*4 + 0] = costheta;
	m[0*4 + 1] = -sintheta;
	m[1*4 + 0] = sintheta;
	m[1*4 + 0] = costheta;
	return m;

}

Matrix4x4 TranslationMatrix(float3 d) {

	Matrix4x4 m = Matrix4x4::identity();
	m[0*4 + 3] = d.x;
	m[1*4 + 3] = d.y;
	m[2*4 + 3] = d.z;
	return m;

}

Matrix4x4 ScalingMatrix(float3 s) {

	Matrix4x4 m = Matrix4x4::identity();
	m[0*4 + 0] = s.x;
	m[1*4 + 1] = s.y;
	m[2*4 + 2] = s.z;
	return m;

}








