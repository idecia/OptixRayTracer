
#pragma once

#include <optixu/optixu_math_stream_namespace.h>
using namespace optix;

class Film {

public:
    
	Film(int xResolution, int yResolution);
	RT_HOSTDEVICE void PutSample(float2 sample, float3 color);
	RT_HOSTDEVICE float2 sample(const uint2 &pixel, const float2 &sample);
	RT_HOSTDEVICE float GetAspectRatio();
	RT_HOSTDEVICE_HOST void OptixSetup(Context context);

private:

	int xResolution;
	int yResolution;
	float aspectRatio;

};

RT_FUNCTION Film::Film() { }

RT_FUNCTION Film::Film(int xResolution, int yResolution)
	: xResolution(xResolution), yResolution(yResolution) {

	aspectRatio = yResolution / xResolution;

}

RT_FUNCTION float2 Film::Sample(const uint2 &pixelIdx, const float2 &uniformSample) {

	float2 filmSample;
	filmSample.x = (pixel.x + uniformSample.x) / (xResolution * 2.f) - 1.f;
	filmSample.y = (pixel.y + uniformSample.y) / (yResolution * 2.f) - 1.f;
	filmSample.y *= aspectRatio;
	return filmSample;
}

RT_FUNCTION float Film::GetAspectRatio() {

	return aspectRatio;

}

RT_HOSTDEVICE_HOST void Film::OptixSetup(Context context) {

	Buffer output = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	output->setFormat(RT_FORMAT_FLOAT3);
	output->setSize(xResolution, yResolution);
	context["output"]->set(output);

}

#ifdef  __CUDA_ARCH__

rtBuffer<float3, 2>   output;

RT_FUNCTION_DEVICE void Film::PutSample(const float2 &sample, const float3 &color)  {

	uint2 pixelIdx;
	pixelIdx.x = (uint1)sample.x;
	pixelIdx.y = (uint1)sample.y;
	output[pixelIdx] = color;

}

#endif