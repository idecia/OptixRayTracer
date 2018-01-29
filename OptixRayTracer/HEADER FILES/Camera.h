#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED


#include <optixu/optixu_math_stream_namespace.h>
using namespace optix;

/*
#include "Vector3.h"
#include <rapidxml\rapidxml.hpp>

using namespace rapidxml;
class Camera
{
   private:

        Vector3 position;
        Vector3 at;
        Vector3 up;
        Vector3 n, u, v;

        float zNear;
        float nearPlaneHeight;
        float nearPlaneWidth;
        float fovy;
        float aspect;

    public:

        void setCamera(Vector3 aPosition, Vector3 aAt, Vector3 aUp);
        void setPerspective(float aFovy, float aAspect, float aZNear);

        float getNearPlaneWidth();
        float getNearPlaneHeight();
        Vector3 getPosition();
        float getZNear();
        float getFovy();
        float getAspect();
        Vector3 getN();
        Vector3 getU();
        Vector3 getV();


        void loadFromXML(xml_node<>* camera);

};

*/

struct Camera {

	float3 eye;
	float3 lookAt;
	float3 up;
	float distance;

	float3 U;
	float3 V;
	float3 W;

};


Camera createCamera(float3 eye, float3 lookAt, float3 up, float distance);
RT_HOSTDEVICE Ray generateRay(Camera &camera, float2 point);

#endif // CAMERA_H_INCLUDED
