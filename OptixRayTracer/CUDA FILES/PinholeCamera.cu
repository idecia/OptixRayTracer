
#include "Camera.h"
#include "ViewPlane.h"

	
#include <optix.h>
#include <optixu/optixu_math_namespace.h>


using namespace optix;

rtBuffer<float3, 2>   outputBuffer;
rtDeclareVariable(uint2, index, rtLaunchIndex, );
rtDeclareVariable(Camera, camera, , );
rtDeclareVariable(ViewPlane, viewPlane, , );
rtDeclareVariable(rtObject, topObject, , );

struct Payload {

	float3 color;

};

RT_PROGRAM void pinholeCamera(void) {

	float2 sample = samplePixel(viewPlane, index.x , index.y);
	Ray ray = generateRay(camera, sample   );
	Payload payload;
	rtTrace(topObject, ray, payload);
	outputBuffer[index] = payload.color;

}