#include "ViewPlane.h"


ViewPlane createViewPlane(int heightResolution, int widthResolution, float pixelSize) {

	ViewPlane viewPlane;
	viewPlane.heightResolution = heightResolution;
	viewPlane.widthResolution = widthResolution;
	viewPlane.pixelSize = pixelSize;
	return viewPlane;

}



RT_HOSTDEVICE optix::float2 samplePixel(ViewPlane &viewPlane, int i, int j) {

	optix::float2 point;
	point.x = viewPlane.pixelSize * (i - 0.5f * viewPlane.widthResolution + 0.5f);
	point.y = viewPlane.pixelSize * (j - 0.5f * viewPlane.heightResolution + 0.5f);
	return point;

}