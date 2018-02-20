#include "ViewPlane.h"


ViewPlane createViewPlane(int heightResolution, int widthResolution, float pixelSize) {

	ViewPlane viewPlane;
	viewPlane.heightResolution = heightResolution;
	viewPlane.widthResolution = widthResolution;
	viewPlane.pixelSize = pixelSize;
	return viewPlane;

}

RT_HOSTDEVICE float2 samplePixel(ViewPlane &viewPlane, int i, int j) {

	float2 point;
	point.x = viewPlane.pixelSize * (i - viewPlane.widthResolution/2 + 0.5f);
	point.y = viewPlane.pixelSize * (j - viewPlane.heightResolution/2 + 0.5f);
	return point;

}