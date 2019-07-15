#pragma once

#include "lights/EnvironmentLight.h"
#include<ctime>
using namespace std;

Scene SceneBuilder::LoadForDC(Context context, const aiScene* scene, int seed) {

	context->setRayTypeCount(2);
	context->setEntryPointCount(1);
	context->setStackSize(2800);

	Scene optixScene(context);
	int width = 1000000;
	int nSamples = 10;

	optixScene.nSamples = nSamples;
	optixScene.width = width;
	//	optixScene.NskyPatches = 146; //145 + 1
	//optixScene.NEnvironmentalPatches = 289; //8193;  //288 + 1;	

	vector<Material> materials;
	loadMaterialsForDC(scene, context, materials);

	vector<aiMesh*> geometryMeshes;
	vector<aiMesh*> areaLightMeshes;
	ClassifyMeshes(scene, geometryMeshes, areaLightMeshes);
	loadGeometryForDC(scene, context, geometryMeshes, materials);

	loadSensorsForDC(context, optixScene, seed);
	return optixScene;

}


void SceneBuilder::loadMaterialsForDC(const aiScene* scene,
	Context &context, vector<Material> &materials) {

	const char* path = "./Reinhart.ptx";
	Program closestHitReinhart = context->createProgramFromPTXFile(path, "closestHit");
	Program anyHitReinhart = context->createProgramFromPTXFile(path, "anyHit");
	Program missReinhart = context->createProgramFromPTXFile(path, "miss");

	context->setMissProgram(RayTypeOpt::REINHART_RADIANCE, missReinhart);

	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

		aiMaterial* material = scene->mMaterials[i];

		aiString name;
		material->Get(AI_MATKEY_NAME, name);

		if (IsMaterialEmissive(material))  {
			Material optixMaterial = context->createMaterial();
			optixMaterial->setClosestHitProgram(RayTypeOpt::REINHART_RADIANCE, closestHitReinhart);
			optixMaterial->setAnyHitProgram(RayTypeOpt::REINHART_SHADOW, anyHitReinhart);
			aiColor3D emissivePower;
			material->Get(AI_MATKEY_COLOR_EMISSIVE, emissivePower);
			float Le[3] = { emissivePower.r, emissivePower.g, emissivePower.b };
			optixMaterial["Le"]->set3fv(Le);
			materials.push_back(optixMaterial);
			continue;
			//ver que hacer aqui
		}

		aiColor3D diffuseColor;
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {

			Material optixMaterial = context->createMaterial();
			optixMaterial->setClosestHitProgram(RayTypeOpt::REINHART_RADIANCE, closestHitReinhart);
			optixMaterial->setAnyHitProgram(RayTypeOpt::REINHART_SHADOW, anyHitReinhart);
			Lambertian brdf(toFloat3(diffuseColor));
			float Le[3] = { 0.0f, 0.0f, 0.0f };
			optixMaterial["Le"]->set3fv(Le);
			optixMaterial["brdf"]->setUserData(sizeof(Lambertian), &brdf);
			//optixMaterial["glass"]->setUint(0);
			materials.push_back(optixMaterial);
			continue;

		}

	}
	
}


void SceneBuilder::loadGeometryForDC(const aiScene* scene,
	Context &context, const vector<aiMesh*> geometryMeshes,
	const vector<Material> &materials) {

	vector<Geometry> geometries;
	loadMeshes(scene, context, geometryMeshes, geometries);

	Group rootNodeGroup = GetGroupFromNode(context, scene, scene->mRootNode, geometries, materials);

	Acceleration acceleration = context->createAcceleration("Sbvh", "Bvh");
	rootNodeGroup->setAcceleration(acceleration);
	acceleration->markDirty(); //_? Estudiar

	context["root"]->set(rootNodeGroup); //ver si esto esta bien

}



void SceneBuilder::loadSensorsForDC(Context &context, Scene &optixScene, int seed) {

	optix::Buffer RNGBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	RNGBuffer->setFormat(RT_FORMAT_USER);
	RNGBuffer->setElementSize(sizeof(RNG));
	RNGBuffer->setSize(optixScene.width);
	RNG* rng = (RNG*)RNGBuffer->map();
	for (unsigned int i = 0; i < optixScene.width; i++) {
		rng[i] = RNG(seed, i );
	}
	RNGBuffer->unmap();
	context["rngs"]->setBuffer(RNGBuffer);


	/*
	//context["sensorPos"]->setFloat(0.0f, 0.1f, 1.75f);
	context["sensorPos"]->setFloat(-421.4f, -288.0f, 8.05f);
	context["sensorNormal"]->setFloat(0.0f, 0.0f, 1.0f);
	//context["sensorPos"]->setFloat(-593.6f, -236.2f, 93.6f);
	//context["sensorNormal"]->setFloat(0.966f, -0.255f, 0.0f);
	*/


	string path = "./PointSensor.ptx";
	Program program = context->createProgramFromPTXFile(path, "sensor");
	context->setRayGenerationProgram(0, program);
	context->setExceptionEnabled(RT_EXCEPTION_ALL, 0);
	Program exception = context->createProgramFromPTXFile(path, "exception");
	context->setExceptionProgram(0, exception);

	context["Ntot"]->setInt(optixScene.width*optixScene.nSamples);
	context["nSamples"]->setInt(optixScene.nSamples);
	unsigned int NskyPatches = optixScene.NskyPatches; //145 + 1


	optix::Buffer coeff = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	coeff->setFormat(RT_FORMAT_FLOAT3);
	coeff->setSize(NskyPatches);
	float* values = (float*)coeff->map();
	for (unsigned int i = 0; i < NskyPatches * 3; i++) {
		values[i] = 0.0f;
	}
	coeff->unmap();
	context["coeff"]->set(coeff);
	context["coeff"]->set(coeff);
	optixScene.coeff = coeff;



}