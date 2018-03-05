
#include "optix_global.h"
#include "films/Film.h"
#include "cameras/Pinhole.h"
#include "lights/PointLight.h"
#include "core/RNG.h"
#include "core/Ray.h"


Context CreateContext() {

	Context context = Context::create();
	context->setRayTypeCount(RayType::RAY_TYPE_COUNT);
	context->setEntryPointCount(1);
	//context["SCENE_EPSILON"]->setFloat(1.e-4f);
	//context->setStackSize(2800);
	return context;
}

Pinhole CreateCamera(Context context) {

	float3 eye = make_float3(50.0f, 52.0f, 295.6f);
	float3 lookAt = normalize(make_float3(0.0f, -0.042612.0f, -1.0f));
	float3 up = make_float3(0.0f, 1.0f, 0.f);
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

	float3 position = make_float3(0.0f, 0.0f, 0.0f);
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
	

void createGeometry() {

	Sphere left(make_float3(1e5 + 1, 40.8, 81.6), 1e5);
	Sphere right(make_float3(-1e5 + 99, 40.8, 81.6), 1e5);
	Sphere back(make_float3(50, 40.8, 1e5), 1e5);
	Sphere front(make_float3(50, 40.8, -1e5 + 170), 1e5);
	Sphere bottom(make_float3(50, 1e5, 81.6), 1e5);
	Sphere top(make_float3(50, -1e5 + 81.6, 81.6), 1e5);
	Sphere floorleft(make_float3(27, 16.5, 47), 16.5);
	Sphere floorright(make_float3(73, 16.5, 78), 16.5);

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
	geometrygroup->setChildCount(8u);
	geometrygroup->setChild(0, leftred.getOptixGeometry(context));
	geometrygroup->setChild(1, rightviolet.getOptixGeometry(context));
	geometrygroup->setChild(2, backwhite.getOptixGeometry(context));
	geometrygroup->setChild(3, frontwhite.getOptixGeometry(context));
	geometrygroup->setChild(4, bottomwhite.getOptixGeometry(context));
	geometrygroup->setChild(5, topwhite.getOptixGeometry(context));
	geometrygroup->setChild(6, floorleftwhite.getOptixGeometry(context));
	geometrygroup->setChild(7, floorrightwhite.getOptixGeometry(context));

	geometrygroup->setAcceleration(context->createAcceleration("NoAccel"));

	context["root"]->set(geometryGroup);
	context["top_shadower"]->set(geometrygroup);

}
	

void CreateRNGS(Context context, int width, int height) {

	Buffer RNGBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	RNGBuffer->setFormat(RT_FORMAT_USER);
	RNGBuffer->setElementSize(sizeof(RNG));
	RNGBuffer->setSize(width, height)
	RNG* rng = (RNG*)rngs->map();
	for (unsigned int i = 0; i < width * height; i++) {
		rng[i] = RNG(0u, i);
	}
	RNGBuffer->unmap();
	context["rngs"]->setBuffer(RNGBuffer);
}



int main(int argc, char* argv[]) {

	Context context = CreateContext();
	Pinhole camera = CreateCamera(context);
	Film film = CreateFilm(context);
	CreateLights(context);
	CreateGeometry(context);

	int width = 512;
	int height = 512;
	CreateRNGS(context, width, height);
	rtContextValidate(context);
	rtContextLaunch2D(0, width, height);

	RTBuffer image = Film.getBuffer();

	rtContextDestroy(context);

}

