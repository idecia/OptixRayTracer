
#include "films/Film.h"
#include "cameras/Pinhole.h"
#include "core/optix_global.h"
#include "lights/PointLight.h"
#include "core/RNG.h"
#include "core/Ray.h"
#include "shapes/Sphere.h"
#include "brdfs/Lambertian.h"
#include "materials/Matte.h"
#include "geometry/MatteSphere.h"
#include "helpers/sutil/sutil.h"
#include "helpers/sutil/GL/glew.h"
#include "helpers/freeglut/include/GL/freeglut.h"

Context CreateContext() {

	Context context = Context::create();
	context->setRayTypeCount(RayType::RAY_TYPE_COUNT);
	context->setEntryPointCount(1);
	//context["SCENE_EPSILON"]->setFloat(1.e-4f);
	//context->setStackSize(2800);
	return context;
}

Pinhole CreateCamera(Context context) {

	float3 eye = make_float3(50.0f, 40.8f, 168.0f);
	float3 lookAt = normalize(make_float3(50.0f, 40.8f, 0.0f));
	float3 up = make_float3(0.0f, 1.0f, 0.0f);
	float distance = 1.0f;
	Pinhole camera(eye, lookAt, up, distance);
	camera.OptixSetup(context);
	return camera;

}

Film CreateFilm(Context context) {

	Film film(640, 480);
	film.OptixSetup(context);
	return film;

}

void CreateLights(Context context) {

	float3 position = make_float3(50.0f, 80.0f, 81.6f);
	float3 color = make_float3(1.0f, 1.0f, 1.0f);
	PointLight light1(position, color);

	PointLight lights[] = { light1 };

	Buffer lightBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
		RT_FORMAT_USER, sizeof( lights ) / sizeof( PointLight ));
	lightBuffer->setElementSize(sizeof( PointLight ));
	memcpy(lightBuffer->map(), lights, sizeof(lights));
	lightBuffer->unmap();
	context["lights"]->setBuffer(lightBuffer);
}
	

void CreateGeometry(Context context) {

	Sphere left(      make_float3(-1e5  + 1,        40.8,       81.6),   1e5);
	Sphere right(     make_float3( 1e5 + 99,        40.8,       81.6),   1e5);
	Sphere back(      make_float3(       50,        40.8,       -1e5),   1e5);   
	Sphere front(     make_float3(       50,        40.8,  1e5 + 170),   1e5);
	Sphere bottom(    make_float3(       50,        -1e5,       81.6),   1e5);
	Sphere top(       make_float3(       50,  1e5 + 81.6,       81.6),   1e5);
	Sphere floorleft( make_float3(       27,        16.5,         47),   16.5);
	Sphere floorright(make_float3(       73,        16.5,         78),   16.5);

	Lambertian whiteBRDF(make_float3(0.75, 0.75, 0.75));
	Lambertian redBRDF(make_float3(0.75, 0.25, 0.25));
	Lambertian violetBRDF(make_float3(0.25, 0.25, 0.75));

	Matte white = Matte(&whiteBRDF);
	Matte red = Matte(&redBRDF);
	Matte violet = Matte(&violetBRDF);

	MatteSphere leftred(&left, &red);
	MatteSphere rightviolet(&right, &violet);
	MatteSphere backwhite(&back, &white);
	MatteSphere frontwhite(&front, &white);
	MatteSphere bottomwhite(&bottom, &white);
	MatteSphere topwhite(&top, &white);
	MatteSphere floorleftwhite(&floorleft, &white);
	MatteSphere floorrightwhite(&floorright, &white);

	GeometryGroup geometryGroup = context->createGeometryGroup();
	geometryGroup->setChildCount(8u);
	geometryGroup->setChild(0, leftred.GetOptixGeometry(context));
	geometryGroup->setChild(1, rightviolet.GetOptixGeometry(context));
	geometryGroup->setChild(2, backwhite.GetOptixGeometry(context));
	geometryGroup->setChild(3, frontwhite.GetOptixGeometry(context));
	geometryGroup->setChild(4, bottomwhite.GetOptixGeometry(context));
	geometryGroup->setChild(5, topwhite.GetOptixGeometry(context));
	geometryGroup->setChild(6, floorleftwhite.GetOptixGeometry(context));
	geometryGroup->setChild(7, floorrightwhite.GetOptixGeometry(context));

	geometryGroup->setAcceleration(context->createAcceleration("NoAccel"));

	context["root"]->set(geometryGroup);

}
	

void CreateRNGS(Context context, int width, int height) {

	Buffer RNGBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	RNGBuffer->setFormat(RT_FORMAT_USER);
	RNGBuffer->setElementSize(sizeof(RNG));
	RNGBuffer->setSize(width, height);
	RNG* rng = (RNG*)RNGBuffer->map();
	for (unsigned int i = 0; i < width * height; i++) {
		rng[i] = RNG(0u, i);
	}
	RNGBuffer->unmap();
	context["rngs"]->setBuffer(RNGBuffer);
}

void glutInitialize(int* argc, char** argv,int width, int height)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OptixRayTracer");
	glutHideWindow();
}

int main(int argc, char* argv[]) {

	int width = 512;
	int height = 512;
	glutInitialize(&argc, argv,width,height);

	Context context = CreateContext();
	Pinhole camera = CreateCamera(context);
	Film film = CreateFilm(context);
	CreateLights(context);
	CreateGeometry(context);

	CreateRNGS(context, width, height);

	context->validate();
	//context->launch(0, width, height);

	Buffer bufferImage = film.GetBuffer(context);
    sutil::displayBufferGL(bufferImage);

	context->destroy();
	

}

