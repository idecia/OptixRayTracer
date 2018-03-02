
#pragma once

#include <optixu/optixu_math_stream_namespace.h>
using namespace optix;

class Film {

public:
    
	Film(int xResolution, int yResolution);
	RT_HOSTDEVICE void PutSample(float2 sample, float3 color);
	RT_HOSTDEVICE float2 sample(const uint2 &pixel, const float2 &sample);
	RT_HOSTDEVICE float GetAspectRatio()

private:

	int xResolution;
	int yResolution;
	float aspectRatio;

};

