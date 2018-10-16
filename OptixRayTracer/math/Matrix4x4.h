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

Matrix4x4 RotationXMatrix(float theta) {

	float cost = cos(theta);
	float sint = sin(theta);
	float m[] = {  
	   1.0f, 0.0f,  0.0f, 0.0f,
	   0.0f, cost, -sint, 0.0f,
	   0.0f, sint,  cost, 0.0f,
	   0.0f, 0.0f,  0.0f, 1.0f   
	};
	return Matrix4x4(m);

}

Matrix4x4 RotationYMatrix(float theta) {

	float cost = cos(theta);
	float sint = sin(theta);
	float m[] = {
		 cost, 0.0f, sint, 0.0f,
		 0.0f, 1.0f, 0.0f, 0.0f,
		-sint, 0.0f, cost, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f
	};
	return Matrix4x4(m);

}

Matrix4x4 RotationZMatrix(float theta) {

	float cost = cos(theta);
	float sint = sin(theta);
	float m[] = {
		cost, -sint, 0.0f, 0.0f,
		sint,  cost, 0.0f, 0.0f,
		0.0f,  0.0f, 1.0f, 0.0f,
		0.0f,  0.0f, 0.0f, 1.0f
	};
	return Matrix4x4(m);
}

Matrix4x4 TranslationMatrix(float3 d) {

	float m[] = {
		1.0f, 0.0f, 0.0f, d.x,
		0.0f, 1.0f, 0.0f, d.y,
		0.0f, 0.0f, 1.0f, d.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return Matrix4x4(m);

}

Matrix4x4 ScalingMatrix(float3 s) {

	float m[] = {
		 s.x, 0.0f, 0.0f, 0.0f,
		0.0f,  s.y, 0.0f, 0.0f,
		0.0f, 0.0f,  s.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return Matrix4x4(m);

}








