#pragma once

#include "core/optix_global.h"

class TriangleMesh {

public:

	RT_FUNCTION TriangleMesh();
	RT_FUNCTION TriangleMesh(RTBuffer1D vertices, RTBuffer1D normals, float area);
	RT_FUNCTION Sample(RNG &rng, float3 &point, float3 &normal);
	RT_FUNCTION float Area();

private:
	
	RTBuffer1D  vertexBuffer;
	RTBuffer1D  normalBuffer;
	float area;


};

RT_FUNCTION TriangleMesh::TriangleMesh() {

}

RT_FUNCTION TriangleMesh::TriangleMesh(RTBuffer1D vertices, RTBuffer1D normals) 
	: vertexBuffer(vertices), normalBuffer(normals), area(area) {

}


RT_FUNCTION Sample(RNG &rng, float3 &point, float3 &normal) {

	int n =  vertexBuffer.size();
	uint32_t ID = rng.RandomUInt(n);
	const float3 &a = vertexBuffer[ID];
	const float3 &b = vertexBuffer[ID + 1];
	const float3 &c = vertexBuffer[ID + 2];
	Triangle triangle(a, b, c);
	point = triangle.Sample(rng.RandomFloat(), rng.RandomFloat());
	normal = normalNBuffer[ID];

}


