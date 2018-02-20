#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include "Camera.h"
#include "Film.h"


using namespace optix;

rtBuffer<float3, 2>   outputBuffer;
rtDeclareVariable(uint2, index, rtLaunchIndex, );
rtDeclareVariable(Camera, camera, , );
rtDeclareVariable(Film, film, , );
rtDeclareVariable(rtObject, root, , );

struct Payload {

	float3 color;

};

RT_PROGRAM void pinhole(void) {

	float2 sample = //TODO
		Ray ray = camera.generateRay(sample);
	Payload payload;
	rtTrace(root, ray, payload);
	film.put(sample, payload.color)

}