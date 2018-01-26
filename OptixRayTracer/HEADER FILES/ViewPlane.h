
#include <optixu/optixu_math_stream_namespace.h>
using namespace optix;


struct ViewPlane {

	int heightResolution;
	int widthResolution;
	float pixelSize;

};


ViewPlane createViewPlane(int heightResolution, int widthResolution, float pixelSize);
RT_HOSTDEVICE  optix::float2 samplePixel(ViewPlane &viewPlane, int i, int j); optix::float2 samplePixel(ViewPlane &viewPlane, int i, int j);