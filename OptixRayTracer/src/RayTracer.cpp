
#include <optix.h>
using namespace optix;


RTcontext context;
RTBuffer output;
unsigned int entry_point = 0;
unsigned int width       = 680;
unsigned int height      = 480;

void createContext() {

	rtContextCreate(&context);
	rtContextSetEntryPointCount(context, 1);

	char* pathToPtx = "./PinholeCamera.ptx";
	RTProgram pinhole;
	rtProgramCreateFromPTXFile(context, pathToPtx, "pinholeCamera", &pinhole);
	rtContextSetRayGenerationProgram(context, 0, pinhole);

	rtBufferCreate(context, RT_BUFFER_INPUT_OUTPUT, &output);
	rtBufferSetFormat(RT_FORMAT_FLOAT4);
	rtBufferSetSize2D(output, width, height);

}

void createGeometry() {

	RTGeometry sphere;
	rtGeometryCreate(context, &sphere);
	rtGeometrySetPrimitiveCount(sphere, 1);

	char* pathToPtx = "./Sphere.ptx";
	RTProgram sphereIntersection;
	rtProgramCreateFromPTXFile(context, pathToPtx, "sphereIntersection", &sphereIntersection);
	rtGeometrySetIntersectionProgram(geometry, sphereInstersection);

	RTProgram sphereBounds;
	rtProgramCreateFromPTXFile(context, pathToPtx, "sphereBounds", &sphereBounds);
	rtGeometrySetBoundingBoxProgram(sphere, sphereBounds);
	
	RTVariable radius;
	rtGeometryDeclareVariable(geometry, "radius", radius);
	rtVariableSet1f(radius,10.0f);
	
}

void createMaterial() {



}


void saveImage() {


}


void destroyContext(RTContext &context) {

	rtContextDestroy(context);

}

void launch() {
	rtContextValidate(context);
	rtContextLaunch2D(entry_point, width, height);
}

int main(int argc, char* argv[]) {

	createContext();
	createGeometry();
	createMaterial();
	createInstance();
	launch();
	saveImage();
	destroyContext();

}

