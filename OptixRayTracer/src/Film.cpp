#include "Film.h"


Film::Film(int xResolution, int yResolution)
	: xResolution(xResolution), yResolution(yResolution) {

	aspectRatio = yResolution / xResolution;

}

RT_HOSTDEVICE float2 sample(const uint2 &pixel, const float2 &sample){

   float2 point;
   point.x = (pixel.x + sample.x) / (xResolution * 2.f) - 1.f;
   point.y =  pixel.y + sample.y) / (yResolution * 2.f) - 1.f;
   point.y *= aspectRatio;
   return point;
}


RT_HOSTDEVICE float Film::GetAspectRatio() {
	return aspectRatio;
}


RT_HOSTDEVICE void Film::PutSample(float2 sample, float3 color) {


}
