#pragma once

#include "core/optix_global.h"

class TriangleMesh {

public:

	RT_FUNCTION TriangleMesh();
	RT_FUNCTION TriangleMesh(RTBuffer1D vertices, RTBuffer1D normals, float area);
	RT_FUNCTION void Sample(RNG &rng, float3 &point, float3 &normal);
	RT_FUNCTION float GetArea();

private:
	
	RTBuffer1D  vertexBuffer;
	RTBuffer1D  normalBuffer;
	float area;


};

RT_FUNCTION TriangleMesh::TriangleMesh() {

}

RT_FUNCTION TriangleMesh::TriangleMesh(RTBuffer1D vertices, RTBuffer1D normals, float area) 
	: vertexBuffer(vertices), normalBuffer(normals), area(area) {

}

RT_FUNCTION float TriangleMesh::GetArea() {

	return area;
}

#ifdef  __CUDA_ARCH__

RT_FUNCTION void TriangleMesh::Sample(RNG &rng, float3 &point, float3 &normal) {

	int n =  vertexBuffer.size();
	uint32_t triID = rng.RandomUInt(n);
	const float3 &a = vertexBuffer[triID];
	const float3 &b = vertexBuffer[triID + 1];
	const float3 &c = vertexBuffer[triID + 2];
	Triangle triangle(a, b, c);
	float2 u = make_float2(rng.RandomFloat(), rng.RandomFloat());
	point = triangle.Sample(u);
	normal = normalBuffer[triID];

}

#endif
