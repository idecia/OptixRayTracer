
#include "films/Film.h"
#include "cameras/Pinhole.h"
#include "core/optix_global.h"
#include "lights/AllLights.h"
#include "samplers/RNG.h"
#include "core/Ray.h"
#include "shapes/Sphere.h"
#include "skyes/Reinhart145.h"
#include "shapes/Parallelogram.h"
#include "bxdfs/Lambertian.h"
#include "materials/Matte.h"
#include "shapes/MatteSphere.h"
#include "shapes/Triangle.h"
#include "shapes/MatteTriangle.h"
#include "shapes/MatteParallelogram.h"
//#include "3rdparty/sutil/sutil.h"
//#include "3rdparty/sutil/GL/glew.h"
//#include "3rdparty/freeglut/include/GL/freeglut.h"
#include "core/Scene.h"
#include "core/SceneBuilder.h"
#include "skyes/Beckers288.h"
#include "geometry\Polygon2D.h"
#include "geometry\Mesh2D.h"
#include "geometry\Mesh3D.h"
#include "geometry\Face.h"
#include "exporters/obj.h"
#include "blinds/RectangularBlind.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <chrono>
using namespace std;
using namespace std::chrono;

Scene scene;

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

   AreaLight light1(parallelogram, make_float3(15.0f, 15.0f, 5.0f));

   AreaLight lights[] = { light1 };

   float3 position = make_float3(280, 500.0f, 300.0f);
   float3 color = make_float3(10005.0f, 10005.0f, 10005.0f);
   PointLight light2(position, color);

   vector<char*> ptrs;
   optix::Buffer lightBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
      RT_FORMAT_USER, 2);
   lightBuffer->setElementSize(MAX_LIGHT_SIZE);
   char* basePtr = (char*)lightBuffer->getDevicePointer(0);

   char* buffer = (char*) lightBuffer->map();
   memcpy(buffer, &light2, sizeof(light2));
   memcpy(buffer + MAX_LIGHT_SIZE, &light1, sizeof(light1));
   lightBuffer->unmap();
 
   ptrs.push_back(basePtr);
  ptrs.push_back(basePtr + MAX_LIGHT_SIZE);
   //context["lightsData"]->setBuffer(lightBuffer);

   optix::Buffer ptrsBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
	   RT_FORMAT_USER, 1);
   ptrsBuffer->setElementSize(sizeof(void*));
   buffer = (char*)ptrsBuffer->map();
   memcpy(buffer, ptrs.data(), sizeof(void*)*2);
   ptrsBuffer->unmap();

   context["lights"]->setBuffer(ptrsBuffer);


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

   Lambertian whiteBRDF(make_float3(0.8f, 0.8f, 0.8f));
   Lambertian greenBRDF(make_float3(0.05f, 0.8f, 0.05f));
   Lambertian redBRDF(make_float3(0.8f, 0.05f, 0.05f));

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

   Triangle triangle(
	   make_float3(240.0f, 0.0f, 559.0f),
	   make_float3(480, 0.0f, 559.0f),
	   make_float3(340.0f, 300.0f, 559.0f), false);

   MatteTriangle matteTriangle(&triangle, &red);

   GeometryGroup geometryGroup = context->createGeometryGroup();
   geometryGroup->setChildCount(11u);
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
   geometryGroup->setChild(10, matteTriangle.GetOptixGeometry(context));

   geometryGroup->setAcceleration(context->createAcceleration("NoAccel"));

   context["root"]->set(geometryGroup);

}
   

void CreateRNGS(Context context, int width, int height) {

   optix::Buffer RNGBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
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
{/*
   glutInit(argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(width, height);
   glutInitWindowPosition(100, 100);
   glutCreateWindow("OptixRayTracer");
   glutHideWindow();*/
}

void glutDisplay()
{
	/*
   sutil::displayBufferGL(scene.GetOutputImage());


   glutSwapBuffers();*/
}


void glutRun()
{/*
   // Initialize GL state                                                            
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, 1, 0, 1, -1, 1);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   int width = scene.GetWidth();
   int height = scene.GetHeight();

   glViewport(0, 0, width, height);

   glutShowWindow();
   glutReshapeWindow(width, height);

   // register glut callbacks
   glutDisplayFunc(glutDisplay);

   glutMainLoop();*/
}

void render(int argc, char* argv[]) {
	/*
	try {
		//glutInitialize(&argc, argv,width,height);
		sutil::initGlut(&argc, argv);


		context = CreateContext();
		Pinhole camera = CreateCamera(context);
		film = CreateFilm(context,width, height);
		CreateLights(context);
		CreateRNGS(context, width, height);
		CreateGeometry(context);

		context->setPrintEnabled(true);
		context->validate();
		context->launch(0, width, height);


		string filename = "./cornell.dae";
		scene = SceneBuilder::BuildFromFile(filename);
		scene.Render();
		glutRun();
		//  Buffer bufferImage = scene.GetOutputImage(); 
		//sutil::displayBufferPPM("image", bufferImage);
		//sutil::displayBufferGlut("dd", bufferImage->get());
		//sutil::displayBufferGL(bufferImage);

		//context->destroy();
	}
	catch (optix::Exception e) {
		std::cout << e.getErrorString();
	}*/

}

void optimize(int argc, char* argv[]) {

	try {

		string filename = "./CityOfficeAForOptimization.obj";
		scene = SceneBuilder::BuildFromFile(filename, 0);
		/*scene.ComputeEnv();
		Buffer coeff = scene.GetEnvValues();
		float3* values = (float3*)coeff->map();
		RTsize RTwidth; RTsize RTheight;
		coeff->getSize(RTwidth, RTheight);
		int width = static_cast<int>(RTwidth);
		int height = static_cast<int>(RTheight);
		for (int i = 0; i < width; i++)  {
			for (int j = 0; j < height; j++) {
				float3 v = values[j*width + i];
				cout << "   " << v.x;
			}
			cout << "\n";
		}
		coeff->unmap();*/
		/*scene.sensorPos = make_float3(0.83, -3.58, 0.7);
		//scene.sensorPos = make_float3(0.0, 0.0, 1000);
		scene.sensorNormal = make_float3(0.0f, 0.0f, 1.0f);
		//scene.context->launch(1, 1);
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		scene.ComputeDCSensors();>
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		Buffer coeff = scene.GetSensorValues();
	
		std::chrono::duration<double> elapsed = t2 - t1;
		cout << elapsed.count() << "\n";
		float3* values = (float3*)coeff->map();
		RTsize RTwidth; RTsize RTheight;
		coeff->getSize(RTwidth, RTheight);
		int width = static_cast<int>(RTwidth);
		int height = static_cast<int>(RTheight);
		for (int i = 0; i < width; i++)  {
			for (int j = 0; j < height; j++) {
			float3 v = values[j*width + i];
cout << "   " << v.x;
			}
			cout << "\n";
		}
		coeff->unmap(); */
		scene.Optimize();
	}
	
	catch (optix::Exception e) {
		std::cout << e.getErrorString();
	}

}

void BruteForceDC(int argc, char* argv[]) {

	try {
		int seed = 0;
		if (argc > 1)
			seed = atoi(argv[1]);
		//cout << seed;
		string filename = "./CityOfficeAForOptimization1PM.obj";
		scene = SceneBuilder::BuildFromFile(filename, seed);
		scene.ComputeDCBruteForce();
	}

	catch (optix::Exception e) {
		std::cout << e.getErrorString();
	}

}




int main(int argc, char* argv[]) {

	//float p = 0.2f, a = 0.785f, w = 0.15f,
//		h = 0.01f, l = 2.0f;  int n = 10;
	 // p = (1.5f - n*w) / (n - 1);
//	RectangularBlind Blind(w, h, l, a, p, n); //ver esto
	//RectangularBlind(float w , float h, float l, float a, float S, int n, float c, float H, float W, float L)
	//RectangularBlind Blind(0.15, 0.005  , 2.0    , 0.5    , 0.2   , 10   , 0.02    ,   1.5  , 2      , 0.25); 
	RectangularBlind Blind(0.15, 0.01, 2.0, 1.57, 0.2, 7, 0.07, 1.5, 2, 0.25);
	ExportModel("blind.obj", Blind.GetMesh());
	//Mesh3D* mesh = Blind.GetMesh(); 
	//optimize(argc, argv);
	//BruteForceDC(argc, argv);
	/*
	Polygon2D* p = new Polygon2D();
	p->AddVertex(make_float2(0.0f, 0.0f));
	p->AddVertex(make_float2(1.0f, 0.0f));
	p->AddVertex(make_float2(1.0f, 1.0f));
	p->AddVertex(make_float2(2.0f, 2.0f));
	p->AddVertex(make_float2(3.0f, 2.0f));
	p->AddVertex(make_float2(3.0f, 3.0f));
	p->AddVertex(make_float2(0.0f, 3.0f));

	Mesh2D* m = p->Triangulate();
	const vector<Face*> f = m->GetFaces();
	for (int i = 0; i < f.size(); i++) {
		const vector<int> ind = f[i]->GetIndices();
		//cout << ind[0] << "  " << ind[1] << "  "<<  ind[2] << "\n";
	}

	Mesh3D* m3D = new Mesh3D();
	m3D->AddVertex(make_float3(0.0f, 0.0f, 0.0f));  //v0
	m3D->AddVertex(make_float3(0.0f, -1.0f, 0.0f)); //v1
	m3D->AddVertex(make_float3(1.0f, -1.0f, 0.0f));//v2
	m3D->AddVertex(make_float3(1.0f, 0.0f, 0.0f));//v3
	m3D->AddVertex(make_float3(0.0f, 0.0f, 1.0f));//v4
	m3D->AddVertex(make_float3(0.0f, -1.0f, 1.0f)); //v5
	m3D->AddVertex(make_float3(1.0f, -1.0f, 1.0f));//v6
	m3D->AddVertex(make_float3(1.0f, 0.0f, 1.0f));//v7
	m3D->AddVertex(make_float3(0.5f, -0.5f, 2.0f));//v8

	Face* fac = new Face(0,1,2,3);
	m3D->AddFace(fac);
	fac = new Face(1, 2, 6, 5);
	m3D->AddFace(fac);
	fac = new Face(2, 3, 7, 6);
	m3D->AddFace(fac);
	fac = new Face(3, 0, 4, 7);
	m3D->AddFace(fac);
	fac = new Face(0, 1, 5, 4);
	m3D->AddFace(fac);
	fac = new Face(4, 5, 6, 7);
	m3D->AddFace(fac);
	fac = new Face(5, 6, 8);
	m3D->AddFace(fac);
	fac = new Face(6, 7, 8);
	m3D->AddFace(fac);
	fac = new Face(7, 4, 8);
	m3D->AddFace(fac);
	fac = new Face(4, 5, 8);
	m3D->AddFace(fac);


	m3D->Triangulate();
	const vector<Face*> f2 = m3D->GetFaces();
	for (int i = 0; i < f2.size(); i++) {
		const vector<int> ind = f2[i]->GetIndices();
		//cout << ind[0] << "  " << ind[1] << "  " << ind[2] << "\n";
	}

	Polygon2D* p2D = new Polygon2D();
	p2D->AddVertex(make_float2(0.0f, 0.0f));  //v0
	p2D->AddVertex(make_float2(1.0f, 0.0f)); //v1
	p2D->AddVertex(make_float2(1.0f, 2.0f));//v2
	p2D->AddVertex(make_float2(-1.0f, 1.0f));//v3
	p2D->AddVertex(make_float2(0.5f, 0.5f));//v4

	m3D = Extrude(p2D, make_float3(1.0f, 1.0f, 1.0f));
	m3D->Triangulate();
	const vector<Face*> f3 = m3D->GetFaces();
	for (int i = 0; i < f3.size(); i++) {
		const vector<int> ind = f3[i]->GetIndices();
		cout << ind[0] << "  " << ind[1] << "  " << ind[2] << "\n";
	}
	ExportModel(".", m3D);*/


	/*
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	Mesh3D* mesh = NULL;
	for (int i = 0; i < 1; i++) {
		float w = 0.15, h = 0.01, l = 2, a = 0.7, p = 0;
		int n = 10;
		RectangularBlind Blind(w, h, l, a, p, n);
		mesh = Blind.GetMesh();
		//delete mesh;
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	std::chrono::duration<double> elapsed = t2 - t1;
	cout << elapsed.count() << "\n";
	ExportModel("model.obj", mesh);*/
	

	try {



		/*

		//sutil::initGlut(&argc, argv);
		string filename = "./LIGHTWELL.obj";
		scene = SceneBuilder::BuildFromFile(filename);
		for (int i = 0; i < 6; i++)
			for (int j = 0; j < 12; j++) {
				float3 p = make_float3(-1.25 + i*0.5, -0.5 - j*0.5, 0.7);
				scene.ChangeSensorPosition(p);
				scene.ResetSensorValues();
				scene.Render();

				Buffer coeff = scene.GetSensorValues();
				float3* values = (float3*)coeff->map();
				RTsize RTwidth; coeff->getSize(RTwidth);
				int width = static_cast<int>(RTwidth);
				for (int i = 0; i < width; i++) {
					float3 v = values[i];
					cout << "   " << v.x * M_PIf /1000000;
				}
				cout << "\n";
				coeff->unmap();
			}
		// glutRun();
		//Buffer image = scene.GetOutputImage();*/
		//float3 v = make_float3(0.5f, 0.5f, 2.5f);
		//cout << reinhart(v, 1);
		

		/* float3 p = make_float3(0.0f, 0.0f, 2.5f);
		 scene.ChangeSensorPosition(p);
		 scene.ResetSensorValues();
		 for (int i = 1; i <= 1; i++)
		 scene.Render();
		 Buffer coeff = scene.GetSensorValues();
		 float3* values = (float3*)coeff->map();
		 RTsize RTwidth; coeff->getSize(RTwidth);
		 int width = static_cast<int>(RTwidth);
		 float3 sum = make_float3(0.0f);
		 for (int i = 0; i < width; i++) {
		 float3 v = values[i];
		 if (i > 0)
		 sum += v;
		 cout << "   " << v.x << "   " << v.y << "   " << v.z;
		 }
		 cout << "\n";
		 cout << sum.x*M_PIf/10000000;
		 coeff->unmap();*/
		/*  float R0 = 0.12f, T0 = 0.85f, d = 0.004, lambda = 898e-9;
		  ThinGlass brdf(R0, T0, d, lambda);
		  for (int i = 0; i <= 90; i++) {
		  float teta = i * (M_PIf/ 180.0f);
		  float f = brdf.F.Transmittance(cosf(teta));
		  cout << f << "\n";
		  }*/

		/*
	   FILE * pFile;
	   pFile = fopen("SensorsIgnacio.txt", "r");
	   float3 p; float d;
	   string filename = "./LIGHTWELL.obj";
	   scene = SceneBuilder::BuildFromFile(filename);
	   // while (fscanf(pFile, "%f\t%f\t%f\t%f\t%f\t%f\n", &p.x, &p.y, &p.z, &d, &d, &d) != EOF) {
	   fscanf(pFile, "%f\t%f\t%f\t%f\t%f\t%f\n", &p.x, &p.y, &p.z, &d, &d, &d);
	   scene.ChangeSensorPosition(p);
	   scene.ResetSensorValues();
	   scene.Render();

	   Buffer coeff = scene.GetSensorValues();
	   float3* values = (float3*)coeff->map();
	   RTsize RTwidth; coeff->getSize(RTwidth);
	   int width = static_cast<int>(RTwidth);
	   for (int i = 0; i < width; i++) {
	   float3 v = values[i];
	   cout << "   " << v.x << "   " << v.y << "   " << v.z;
	   }
	   cout << "\n";
	   coeff->unmap();
	   //}*/

		/*	//string filename = "./LIGHTWELL_DARK_WINDOW.obj";
		 string filename = "./City.obj";
		 scene = SceneBuilder::BuildFromFile(filename);
		 scene.Render();
		 Buffer coeff = scene.GetSensorValues();
		 float3* values = (float3*)coeff->map();
		 RTsize RTwidth; RTsize RTheight;
		 coeff->getSize(RTwidth, RTheight);
		 int width = static_cast<int>(RTwidth);
		 int height = static_cast<int>(RTheight);
		 for (int i = 0; i < width; i++)  {
		 for (int j = 0; j < height; j++) {
		 float3 v = values[j*width + i];
		 cout << "   " << v.x;
		 }
		 cout << "\n";
		 }
		 coeff->unmap();
		 //float3 d = make_float3(0.078184273264963,   0.126575159385289,   0.988871047427630);
		 //cout << beckers(d);

		 }*/

	} 
	
	
   catch (optix::Exception e) {
	   std::cout << e.getErrorString();
   }
   
  
   
   

}

