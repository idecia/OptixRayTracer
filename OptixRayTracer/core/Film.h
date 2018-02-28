
#pragma once

#include "core/optix_global.h"

class Film {

public:

	RT_FUNCTION Film();

	RT_FUNCTION Film(int xResolution, int yResolution);

	RT_FUNCTION float2 Sample(const uint2 &pixelIdx, const float2 &uniformSample);

	RT_FUNCTION float GetAspectRatio();
 
	RT_FUNCTION_DEVICE void PutSample(const float2 &sample, const float3 &color) ;

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

#ifdef  __CUDA_ARCH__

rtBuffer<float3, 2>   output;

RT_FUNCTION_DEVICE void Film::PutSample(const float2 &sample, const float3 &color)  {

	uint2 pixelIdx;
	pixelIdx.x = (uint1)sample.x;
	pixelIdx.y = (uint1)sample.y;
	output[pixelIdx] = color;

}

#endif