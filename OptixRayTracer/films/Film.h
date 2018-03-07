
#pragma once

#include "core/optix_global.h"



class Film {

public:

	RT_FUNCTION Film();
	RT_FUNCTION Film(int xResolution, int yResolution);
	RT_FUNCTION void PutSample(const float2 &sample, const float3 &color);
	RT_FUNCTION float2 Sample(const uint2 &pixel, const float2 &sample) const;
	RT_FUNCTION float GetAspectRatio() const;
	RT_FUNCTION_HOST void OptixSetup(Context context);
	RT_FUNCTION_HOST Buffer GetBuffer(Context context);

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

RT_FUNCTION float2 Film::Sample(const uint2 &pixelIdx, const float2 &uniformSample) const {

	float2 filmSample;
	filmSample.x = (2.0f * (pixelIdx.x + uniformSample.x) / xResolution) - 1.0f;
	filmSample.y = (2.0f * (pixelIdx.y + uniformSample.y) / yResolution) - 1.0f;
	filmSample.y *= aspectRatio;
	return filmSample;
}

RT_FUNCTION float Film::GetAspectRatio() const {

	return aspectRatio;

}

RT_FUNCTION_HOST void Film::OptixSetup(Context context) {

	Buffer output = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	output->setFormat(RT_FORMAT_FLOAT3);
	output->setSize(xResolution, yResolution);
	context["output"]->set(output);

}

RT_FUNCTION_HOST Buffer Film::GetBuffer(Context context) {

	return context["output"]->getBuffer();

}

#ifdef  __CUDA_ARCH__

rtBuffer<float3, 2>   output;

RT_FUNCTION_DEVICE void Film::PutSample(const float2 &sample, const float3 &color)  {

	uint2 pixelIdx;
	pixelIdx.x = (unsigned int) sample.x;
	pixelIdx.y = (unsigned int) sample.y;
	output[pixelIdx] = color;

}

#endif