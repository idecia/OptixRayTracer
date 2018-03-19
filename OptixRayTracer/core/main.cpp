
#include "films/Film.h"
#include "cameras/Pinhole.h"
#include "core/optix_global.h"
#include "lights/PointLight.h"
#include "lights/AreaLight.h"
#include "core/RNG.h"
#include "core/Ray.h"
#include "shapes/Sphere.h"
#include "shapes/Parallelogram.h"
#include "brdfs/Lambertian.h"
#include "materials/Matte.h"
#include "geometry/MatteSphere.h"
#include "geometry/MatteParallelogram.h"
#include "helpers/sutil/sutil.h"
#include "helpers/sutil/GL/glew.h"
#include "helpers/freeglut/include/GL/freeglut.h"
#include <iostream>

Context CreateContext() {

	Context context = Context::create();
	context->setRayTypeCount(RayType::RAY_TYPE_COUNT);
	context->setEntryPointCount(1);
	//context["SCENE_EPSILON"]->setFloat(1.e-4f);
	//context->setStackSize(2800);
	return context;
}

Pinhole CreateCamera(Context context) {

	/*float3 eye = make_float3(50.0f, 40.8f, 160.0f);
	float3 lookAt = make_float3(50.0f, 40.8f, 0.0f);
	float3 up = make_float3(0.0f, 1.0f, 0.0f);
	float distance = 1.0f;
	Pinhole camera(eye, lookAt, up, distance);*/
	Pinhole camera(
		make_float3(278.0f, 273.0f, -900.0f),
		make_float3(278.0f, 273.0f, 0.0f),
		make_float3(0.0f, 1.0f, 0.0f),
		2.3f);

	camera.OptixSetup(context);
	return camera;

}

Film CreateFilm(Context context, int width, int height) {

	Film film(width, height);
	film.OptixSetup(context);
	return film;

}

void CreateLights(Context context) {

	/*Point Light
	float3 position = make_float3(50.0f, 70.0f, 81.6f);
	float3 color = make_float3(1.0f, 1.0f, 1.0f);
	PointLight light1(position, color);

	PointLight lights[] = { light1 };

	Buffer lightBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
		RT_FORMAT_USER, sizeof( lights ) / sizeof( PointLight ));
	lightBuffer->setElementSize(sizeof( PointLight ));
	memcpy(lightBuffer->map(), lights, sizeof(lights));
	lightBuffer->unmap();
	context["lights"]->setBuffer(lightBuffer);
	*/

	/*Area Light */

	/*float3 p0 = make_float3(30, 81, 95);
	float3 p1 = make_float3(70, 81, 95);
	float3 p2 = make_float3(30, 81, 55);
	Parallelogram parallelogram(p0, p1, p2, false);
	float3 color = make_float3(7.0f, 7.0f, 7.0f);
	AreaLight light1(parallelogram, color);*/

	Parallelogram parallelogram(
		make_float3(343.0f, 540.0f, 227.0f),
		make_float3(213.0f, 540.0f, 227.0f),
		make_float3(343.0f, 540.0f, 332.0f), false);

	AreaLight light1(parallelogram, 4.5*make_float3(15.0f, 15.0f, 5.0f));

	AreaLight lights[] = { light1 };


	Buffer lightBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
		RT_FORMAT_USER, sizeof(lights) / sizeof(AreaLight));
	lightBuffer->setElementSize(sizeof(AreaLight));
	memcpy(lightBuffer->map(), lights, sizeof(lights));
	lightBuffer->unmap();
	context["lights"]->setBuffer(lightBuffer);


}
	

void CreateGeometry(Context context) {

	/*
	Sphere left(make_float3(-1e5 + 1, 40.8, 81.6), 1e5);
	Sphere right(make_float3(1e5 + 99, 40.8, 81.6), 1e5);
	Sphere back(make_float3(50, 40.8, -1e5), 1e5);
	Sphere front(make_float3(50, 40.8, 1e5 + 163), 1e5);
	Sphere bottom(make_float3(50, -1e5, 81.6), 1e5);
	Sphere top(make_float3(50, 1e5 + 81.6, 81.6), 1e5);
	Sphere floorleft(make_float3(27, 16.5, 47), 16.5);
	Sphere floorright(make_float3(73, 16.5, 78), 16.5);

	Lambertian whiteBRDF(make_float3(0.9, 0.9, 0.9));
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

	float3 p0 = make_float3(20, 20, 10);
	float3 p1 = make_float3(80, 20, 10);
	float3 p2 = make_float3(20, 40, 10);
	Parallelogram parallelogram(p0, p1, p2, true);
	MatteParallelogram test(&parallelogram, &violet);

	GeometryGroup geometryGroup = context->createGeometryGroup();
	geometryGroup->setChildCount(9u);
	geometryGroup->setChild(0, leftred.GetOptixGeometry(context));
	geometryGroup->setChild(1, rightviolet.GetOptixGeometry(context));
	geometryGroup->setChild(2, backwhite.GetOptixGeometry(context));
	geometryGroup->setChild(3, frontwhite.GetOptixGeometry(context));
	geometryGroup->setChild(4, bottomwhite.GetOptixGeometry(context));
	geometryGroup->setChild(5, topwhite.GetOptixGeometry(context));
	geometryGroup->setChild(6, floorleftwhite.GetOptixGeometry(context));
	geometryGroup->setChild(7, floorrightwhite.GetOptixGeometry(context));
	geometryGroup->setChild(8, test.GetOptixGeometry(context));*/

	Lambertian whiteBRDF(make_float3(0.8, 0.8, 0.8));
	Lambertian greenBRDF(make_float3(0.05, 0.8, 0.05));
	Lambertian redBRDF(make_float3(0.8, 0.05, 0.05));

	Matte white = Matte(&whiteBRDF);
	Matte green = Matte(&greenBRDF);
	Matte red = Matte(&redBRDF);

	Parallelogram floor(
		make_float3(0.0f, 0.0f, 0.0f),
		make_float3(0.0f, 0.0f, 559.2f),
		make_float3(556.0f, 0.0f, 0.0f), true);

	MatteParallelogram floorWhite(&floor, &white);

	Parallelogram ceiling(
		make_float3(0.0f, 548.0f, 0.0f),
		make_float3(556.0f, 548.0f, 0.0f),
		make_float3(0.0f, 548.0f, 559.2f), true);

	MatteParallelogram ceilingWhite(&ceiling, &white);

	Parallelogram back(
		make_float3(0.0f, 0.0f, 559.2f),
		make_float3(0.0f, 548.8f, 559.2f),
		make_float3(556.0f, 0.0f, 559.2f), true);

	MatteParallelogram backWhite(&back, &white);

	Parallelogram right(
		make_float3(0.0f, 0.0f, 0.0f),
		make_float3(0.0f, 548.8f, 0.0f),
		make_float3(0.0f, 0.0f, 559.2f), true);

	MatteParallelogram rightGreen(&right, &green);

	Parallelogram left(
		make_float3(556.0f, 0.0f, 0.0f),
		make_float3(556.0f, 0.0f, 559.2f),
		make_float3(556.0f, 548.8f, 0.0f), true);

	MatteParallelogram leftRed(&left, &red);

	Parallelogram p1(
		make_float3(130.0f, 165.0f, 65.0f),
		make_float3(82.0f, 165.0f, 225.0f),
		make_float3(290.0f, 165.8f, 114.0f), true);
	MatteParallelogram p1W(&p1, &white);

	Parallelogram p2(
		make_float3(290.0f, 0.0f, 114.0f),
		make_float3(290.0f, 165.0f, 114.0f),
		make_float3(240.0f, 0.0f, 272.0f), true);
	MatteParallelogram p2W(&p2, &white);

	Parallelogram p3(
		make_float3(130.0f, 0.0f, 65.0f),
		make_float3(130.0f, 165.0f, 65.0f),
		make_float3(290.0f, 0.0f, 114.0f), false);
	MatteParallelogram p3W(&p3, &white);

	Parallelogram p4(
		make_float3(82.0f, 0.0f, 225.0f),
		make_float3(82.0f, 165.0f, 225.0f),
		make_float3(130.0f, 0.0f, 65.0f), true);
	MatteParallelogram p4W(&p4, &white);

	Parallelogram p5(
		make_float3(240.0f, 0.0f, 272.0f),
		make_float3(240.0f, 165.0f, 272.0f),
		make_float3(82.0f, 0.0f, 225.0f), true);
	MatteParallelogram p5W(&p4, &white);





	GeometryGroup geometryGroup = context->createGeometryGroup();
	geometryGroup->setChildCount(10u);
	geometryGroup->setChild(0, floorWhite.GetOptixGeometry(context));
	geometryGroup->setChild(1, ceilingWhite.GetOptixGeometry(context));
	geometryGroup->setChild(2, backWhite.GetOptixGeometry(context));
	geometryGroup->setChild(3, leftRed.GetOptixGeometry(context));
	geometryGroup->setChild(4, rightGreen.GetOptixGeometry(context));
	geometryGroup->setChild(5, p1W.GetOptixGeometry(context));
	geometryGroup->setChild(6, p2W.GetOptixGeometry(context));
	geometryGroup->setChild(7, p3W.GetOptixGeometry(context));
	geometryGroup->setChild(8, p4W.GetOptixGeometry(context));
	geometryGroup->setChild(9, p5W.GetOptixGeometry(context));

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

	try {
		int width = 760;
		int height = 550;
		//glutInitialize(&argc, argv,width,height);
		sutil::initGlut(&argc, argv);

		Context context = CreateContext();
		Pinhole camera = CreateCamera(context);
		Film film = CreateFilm(context,width, height);
		CreateLights(context);
		CreateRNGS(context, width, height);
		CreateGeometry(context);

		context->setPrintEnabled(true);
		context->validate();
		context->launch(0, width, height);

		Buffer bufferImage = film.GetBuffer(context);
		sutil::displayBufferPPM("image", bufferImage);
		sutil::displayBufferGlut("dd", bufferImage->get());
		sutil::displayBufferGL(bufferImage);

		context->destroy();
	}
	catch (optix::Exception e) {
		std::cout << e.getErrorString();
	}
	

}

