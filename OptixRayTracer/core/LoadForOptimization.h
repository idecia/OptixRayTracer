#pragma once

#include "lights/EnvironmentLight.h"

Scene SceneBuilder::LoadForOptimization(Context context, const aiScene* scene) {

	context->setRayTypeCount(4);
	context->setEntryPointCount(2);
	context->setStackSize(2800);

	Scene optixScene(context);
	int width = 10000;
	int nSamples = 100;
	optixScene.nSamples = nSamples;
	optixScene.width = width;
	optixScene.NskyPatches = 146; //145 + 1
	optixScene.NEnvironmentalPatches = 8193; //289; //288 + 1;	

	vector<Material> materials;
	loadMaterialsForOptimization(scene, context, materials);
	int idxLight;
	loadGeometryForOptimization(scene, context, materials, idxLight);
	loadLightsForOptimization(scene, context, idxLight, optixScene);
	loadSensorsForOptimization(context, optixScene);
	return optixScene;

}


void SceneBuilder::loadMaterialsForOptimization(const aiScene* scene,
	Context &context, vector<Material> &materials) {

	const char* path = "./Reinhart.ptx";
	Program closestHitReinhart = context->createProgramFromPTXFile(path, "closestHit");
	Program anyHitReinhart = context->createProgramFromPTXFile(path, "anyHit");
	Program missReinhart = context->createProgramFromPTXFile(path, "miss");

	path = "./Beckers.ptx";
	Program closestHitBeckers = context->createProgramFromPTXFile(path, "closestHit");
	Program anyHitBeckers = context->createProgramFromPTXFile(path, "anyHit");
	Program missBeckers = context->createProgramFromPTXFile(path, "miss");

	context->setMissProgram(RayTypeOpt::REINHART_RADIANCE, missReinhart);
	context->setMissProgram(RayTypeOpt::BECKERS_RADIANCE, missBeckers);

	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

		aiMaterial* material = scene->mMaterials[i];

		aiString name;
		material->Get(AI_MATKEY_NAME, name);

		if (IsMaterialEmissive(material))  {
			Material optixMaterial = context->createMaterial();
			optixMaterial->setClosestHitProgram(RayTypeOpt::REINHART_RADIANCE, closestHitReinhart);
			optixMaterial->setAnyHitProgram(RayTypeOpt::REINHART_SHADOW, anyHitReinhart);
			optixMaterial->setClosestHitProgram(RayTypeOpt::BECKERS_RADIANCE, closestHitBeckers);
			optixMaterial->setAnyHitProgram(RayTypeOpt::BECKERS_SHADOW, anyHitBeckers);
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
			optixMaterial->setClosestHitProgram(RayTypeOpt::BECKERS_RADIANCE, closestHitBeckers);
			optixMaterial->setAnyHitProgram(RayTypeOpt::BECKERS_SHADOW, anyHitBeckers);
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



void SceneBuilder::loadLightsForOptimization(const aiScene* scene,
	Context &context, int idxLight, Scene &optixScene) {

	aiMesh* mesh = scene->mMeshes[idxLight];
	aiFace face = mesh->mFaces[0];
	if (face.mNumIndices == 3) {
		float3 p0 = toFloat3(mesh->mVertices[face.mIndices[0]]);
		float3 p1 = toFloat3(mesh->mVertices[face.mIndices[1]]);
		float3 p2 = toFloat3(mesh->mVertices[face.mIndices[2]]);
		float3 n = toFloat3(mesh->mNormals[0]);
		optixScene.sensorNormal = -n;
		optixScene.sensorPos = ((p0 + p2) / 2.0f) + 0.1*optixScene.sensorNormal;
		Parallelogram parallelogram(p0, p1, p2, n);


		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		EnvironmentLight light(parallelogram);
		context["light"]->setUserData(sizeof(EnvironmentLight), &light);

	}
}

Group SceneBuilder::GetGroup(optix::Context &context, const vector<GeometryInstance> &instances) {

	GeometryGroup geometryGroup = context->createGeometryGroup(instances.begin(), instances.end());
	Acceleration acceleration = context->createAcceleration("Sbvh", "Bvh");
	acceleration->setProperty("vertex_buffer_name", "vertexBuffer");
	acceleration->setProperty("index_buffer_name", "indexBuffer");
	geometryGroup->setAcceleration(acceleration);
	acceleration->markDirty();

	Group group = context->createGroup();
	group->setChildCount(1);
	group->setChild(0, geometryGroup);
	{
		optix::Acceleration acceleration = context->createAcceleration("NoAccel", "NoAccel");
		group->setAcceleration(acceleration);
	}

	return group;

}

void SceneBuilder::loadGeometryForOptimization(const aiScene* scene,
	Context &context, const vector<Material> &materials, int& idxLight) {

	vector<Geometry> geometries;

	vector<GeometryInstance> cityInst;
	vector<GeometryInstance> buildingInst;
	vector<GeometryInstance> windowInst;

	loadMeshesForOptimization(scene, context, materials, cityInst, buildingInst, windowInst, idxLight);

	Group cityGroup = GetGroup(context, cityInst);
	Group buildingGroup = GetGroup(context, buildingInst);
	Group windowGroup = GetGroup(context, windowInst);

	Group buildingWindowGroup = context->createGroup();
	buildingWindowGroup->setChildCount(2);
	buildingWindowGroup->setChild(0, buildingGroup);
	buildingWindowGroup->setChild(1, windowGroup);
	{
		//ver si es conveniente agregar una estructura de aceleracion aqui
		optix::Acceleration acceleration = context->createAcceleration("NoAccel", "NoAccel");
		buildingWindowGroup->setAcceleration(acceleration);
		acceleration->markDirty();
	}
	context["buildingWindows"]->set(buildingWindowGroup);

	Group root = context->createGroup();
	root->setChildCount(2);
	root->setChild(0, buildingWindowGroup);
	root->setChild(1, cityGroup);
	{
		optix::Acceleration acceleration = context->createAcceleration("Sbvh", "Bvh");
		root->setAcceleration(acceleration);
		acceleration->markDirty(); 
	}
	context["root"]->set(root); //ver si esto esta bien

}

void SceneBuilder::loadMeshesForOptimization(const aiScene* scene,
	Context &context, const vector<Material> &materials, vector<GeometryInstance> &city, 
	vector<GeometryInstance> &building, vector<GeometryInstance> &window, int &idxLight) {

	const char* path = "./Mesh.ptx";
	Program bboxProg = context->createProgramFromPTXFile(path, "boundingBox");
	Program intersecProg = context->createProgramFromPTXFile(path, "intersect");

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

		aiMesh* mesh = scene->mMeshes[i];
		string name(mesh->mName.C_Str());

		unsigned int numFaces = mesh->mNumFaces;
		unsigned int numVertices = mesh->mNumVertices;

		optix::Geometry geometry = context->createGeometry();
		geometry->setPrimitiveCount(numFaces);
		geometry->setIntersectionProgram(intersecProg);
		geometry->setBoundingBoxProgram(bboxProg);

		GeometryInstance gi = GetGeometryInstance(context, geometry, materials[mesh->mMaterialIndex]);
		if (name.find("city") == 0)
			city.push_back(gi);
		else if (name.find("building") == 0)
			building.push_back(gi);
		else if (name.find("window") == 0) {
			window.push_back(gi);
			idxLight = i;
		}

		Buffer vertexBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, numVertices);
		float3* vertexBuffer_Host = static_cast<optix::float3*>(vertexBuffer->map());

		Buffer normalBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, numVertices);
		float3* normalBuffer_Host = static_cast<optix::float3*>(normalBuffer->map());

		geometry["vertexBuffer"]->setBuffer(vertexBuffer);
		geometry["normalBuffer"]->setBuffer(normalBuffer);

		memcpy(static_cast<void*>(vertexBuffer_Host),
			static_cast<void*>(mesh->mVertices),
			sizeof(optix::float3)*numVertices);
		vertexBuffer->unmap();

		/*for (int i = 1; i < numVertices; i++) {
		float x = mesh->mNormals[i].x;
		float y = mesh->mNormals[i].y;
		float z = mesh->mNormals[i].z;
		x = mesh->mVertices[i].x;
		y = mesh->mVertices[i].y;
		z = mesh->mVertices[i].z;
		}*/

		memcpy(static_cast<void*>(normalBuffer_Host),
			static_cast<void*>(mesh->mNormals),
			sizeof(optix::float3)*numVertices);
		normalBuffer->unmap();

		//[TODO] Texture buffer
		//[TODO] Tangent and bitangent buffers

		// Create index buffer
		optix::Buffer indexBuffer = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_INT3, numFaces);
		optix::int3* indexBuffer_Host = static_cast<optix::int3*>(indexBuffer->map());
		geometry["indexBuffer"]->setBuffer(indexBuffer);

		// Copy index buffer from host to device
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			indexBuffer_Host[i].x = face.mIndices[0];
			indexBuffer_Host[i].y = face.mIndices[1];
			indexBuffer_Host[i].z = face.mIndices[2];
		}

		indexBuffer->unmap();

	}

}

void SceneBuilder::loadSensorsForOptimization(Context &context, Scene &optixScene) {

	optix::Buffer RNGBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	RNGBuffer->setFormat(RT_FORMAT_USER);
	RNGBuffer->setElementSize(sizeof(RNG));
	RNGBuffer->setSize(optixScene.width);
	RNG* rng = (RNG*)RNGBuffer->map();
	for (unsigned int i = 0; i < optixScene.width; i++) {
		rng[i] = RNG(0u, i + 915711);
	}
	RNGBuffer->unmap();
	context["rngs"]->setBuffer(RNGBuffer);

	const char* path = "./Environmental.ptx";
	Program program = context->createProgramFromPTXFile(path, "sensor");
	context->setRayGenerationProgram(0, program);
	context->setExceptionEnabled(RT_EXCEPTION_ALL, 1);
	Program exception = context->createProgramFromPTXFile(path, "exception");
	context->setExceptionProgram(0, exception);

	/*
	//context["sensorPos"]->setFloat(0.0f, 0.1f, 1.75f);
	context["sensorPos"]->setFloat(-421.4f, -288.0f, 8.05f);
	context["sensorNormal"]->setFloat(0.0f, 0.0f, 1.0f);
	//context["sensorPos"]->setFloat(-593.6f, -236.2f, 93.6f);
	//context["sensorNormal"]->setFloat(0.966f, -0.255f, 0.0f);
	*/
	
	context["Ntot"]->setInt(optixScene.width);
	context["nSamples"]->setInt(optixScene.nSamples);
	optix::Buffer environmentMap = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	environmentMap->setFormat(RT_FORMAT_FLOAT3);
	unsigned int NskyPatches = optixScene.NskyPatches; //145 + 1
	unsigned int NEnvironmentalPatches = optixScene.NEnvironmentalPatches; //288 + 1;
	//unsigned int NEnvironmentalPatches = 4097;
	environmentMap->setSize(NEnvironmentalPatches, NskyPatches);
	float* values = (float*)environmentMap->map();
	for (unsigned int i = 0; i < NEnvironmentalPatches*NskyPatches * 3; i++) {
		values[i] = 0.0f;
	}
	environmentMap->unmap();
	context["env"]->set(environmentMap);
	optixScene.env = environmentMap;


	path = "./PointSensorBeckers.ptx";
	program = context->createProgramFromPTXFile(path, "sensor");
	context->setRayGenerationProgram(1, program);

	/*
	context["sensorPos"]->setFloat(-1.25f, -0.5f, 0.70f);
	//context["sensorPos"]->setFloat(100000000.0f, 100000000.0f, 100000000.0f);
	context["sensorNormal"]->setFloat(0.0f, 0.0f, 1.0f);
	context["N"]->setInt(width);
	*/

	optix::Buffer coeff = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	coeff->setFormat(RT_FORMAT_FLOAT3);
	coeff->setSize(NEnvironmentalPatches);
	values = (float*)coeff->map();
	for (unsigned int i = 0; i < NEnvironmentalPatches * 3; i++) {
		values[i] = 0.0f;
	}
	coeff->unmap();
	context["coeff"]->set(coeff);
	optixScene.coeff = coeff;

	
}