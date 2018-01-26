  #include "Camera.h"
  #include <string>
  #include <math.h>

/*
  #define PI 3.14159265

  void Camera::setCamera(Vector3 aPosition, Vector3 aAt, Vector3 aUp)
  {
      position = aPosition;
      at = aAt.normalize();
      up = aUp.normalize();

      n = (-1.0)*at;
      u = crossProduct(up,n).normalize();
      v = crossProduct(n,u);

  }

void Camera::setPerspective(float aFovy, float aAspect, float aZNear)
{
    fovy    = aFovy;
    aspect  = aAspect;
    zNear   = aZNear;

    float toRadians =  PI / 180.0;
    float fovyInRadians = fovy * toRadians;
    nearPlaneHeight = 2*tan(fovyInRadians/ 2.0)*zNear;
    nearPlaneWidth  = aspect* nearPlaneHeight;
}


float Camera::getNearPlaneWidth()
{
    return nearPlaneWidth;
}

float Camera::getNearPlaneHeight()
{
    return nearPlaneHeight;
}

Vector3 Camera::getN()
{
    return n;
}

Vector3 Camera::getU()
{
    return u;
}

Vector3 Camera::getV()
{
    return v;
}

Vector3 Camera::getPosition()
{
    return position;
}

float Camera::getAspect()
{
    return aspect;
}


float Camera::getZNear()
{
    return zNear;
}


float Camera::getFovy()
{
    return fovy;
}



void Camera::loadFromXML(xml_node<>* camera){
    Vector3 cameraPosition = xmlToVector3(camera->first_node("position"));
    Vector3 cameraAt= xmlToVector3(camera->first_node("lookAt"));
    Vector3 cameraUp = xmlToVector3(camera->first_node("up"));
    setCamera(cameraPosition,cameraAt,cameraUp);

    float fovy   = xmlToFloat(camera->first_node("fov"));
    float aspect = xmlToFloat(camera->first_node("aspect"));
    float zNear  = xmlToFloat(camera->first_node("zNear"));
    setPerspective(fovy, aspect, zNear);


}


*/

void computeUVW(Camera &camera, float3 eye, float3 lookAt, float3 up) {

	float3 w = normalize(eye - lookAt);
	float3 u = normalize(cross(up, u));
	float3 v = cross(w, u);
	camera.w = w;
	camera.u = u;
	camera.v = v;
}

Camera cameraCreate(float3 eye, float3 lookAt, float3 up, float distance) {
	

	Camera camera;
	camera.eye = eye;
	camera.lookAt = lookAt;	
	camera.up = up;
	camera.distance = distance;
	computeUVW(camera, eye, lookAt, up);

	return camera;
}

RT_HOSTDEVICE optix::Ray generateRay(Camera &camera, float2 point) {

	float3 direction = point.x * camera.u + point.y * camera.v - camera.distance * camera.w;
	direction = normalize(direction);

	optix::Ray ray = make_Ray(camera.eye, direction, 0, 0.05f, RT_DEFAULT_MAX);
	return ray;
}


