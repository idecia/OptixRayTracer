#pragma once

Scene SceneBuilder::LoadForRender(Context context, const aiScene* scene) {

	context->setRayTypeCount(RayType::RAY_TYPE_COUNT);
	context->setEntryPointCount(1);
	context->setStackSize(2800);


	vector<Material> materials;
	loadMaterialsForRender(scene, context, materials);

	vector<aiMesh*> geometryMeshes;
	vector<aiMesh*> areaLightMeshes;
	ClassifyMeshes(scene, geometryMeshes, areaLightMeshes);

	vector<Light*> lights;
	loadLightsForRender(scene, context, areaLightMeshes, lights);

	loadGeometryForRender(scene, context, geometryMeshes, materials);
	//ver si es necesario calcular el ABBox de la escena

	int width = 1024;
	int height = 1024;
	loadCamera(scene, context, width, height);
	//optix::Buffer coeff = loadSensors(scene, context, width);
	//Scene optixScene(context, coeff);
	Scene optixScene(context);
	optixScene.SetWidth(width);
	optixScene.SetHeight(height);
	return optixScene;

}


void SceneBuilder::loadMaterialsForRender(const aiScene* scene,
	Context &context, vector<Material> &materials) {

	const char* path = "./PathTracer.ptx";
	Program closestHitRadiance = context->createProgramFromPTXFile(path, "closestHit");
	Program anyHit = context->createProgramFromPTXFile(path, "anyHit");
	Program miss = context->createProgramFromPTXFile(path, "miss");
	context->setMissProgram(RayType::RADIANCE, miss);

	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

		aiMaterial* material = scene->mMaterials[i];

		aiString name;
		material->Get(AI_MATKEY_NAME, name);

		if (IsMaterialEmissive(material))  {
			Material optixMaterial = context->createMaterial();
			optixMaterial->setClosestHitProgram(RayType::RADIANCE, closestHitRadiance);
			optixMaterial->setAnyHitProgram(RayType::SHADOW, anyHit);
			aiColor3D emissivePower;
			material->Get(AI_MATKEY_COLOR_EMISSIVE, emissivePower);
			float Le[3] = { emissivePower.r, emissivePower.g, emissivePower.b };
			optixMaterial["Le"]->set3fv(Le);
			materials.push_back(optixMaterial);
			continue;
			//ver que hacer aqui
		}
		//[TODO] Textured material

		//[TODO] Reflective/mirror material
		float indexOfRefraction;
		if (material->Get(AI_MATKEY_REFRACTI, indexOfRefraction) == AI_SUCCESS && indexOfRefraction > 1.0f) {
			//printf("\tGlass: IOR: %g\n", indexOfRefraction);
			Material optixMaterial = context->createMaterial();
			optixMaterial->setClosestHitProgram(RayType::RADIANCE, closestHitRadiance);
			optixMaterial->setAnyHitProgram(RayType::SHADOW, anyHit);
			float R0 = 0.12f, T0 = 0.78f, d = 0.004, lambda = 898e-9;//575e-9;
			ThinGlass brdf(R0, T0, d, lambda);
			optixMaterial["brdfGlass"]->setUserData(sizeof(ThinGlass), &brdf);
			materials.push_back(optixMaterial);
			optixMaterial["glass"]->setUint(1);
			continue;
		}

		aiColor3D diffuseColor;
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {

			Material optixMaterial = context->createMaterial();
			optixMaterial->setClosestHitProgram(RayType::RADIANCE, closestHitRadiance);
			optixMaterial->setAnyHitProgram(RayType::SHADOW, anyHit);
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


void SceneBuilder::loadGeometryForRender(const aiScene* scene,
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


void SceneBuilder::loadCamera(const aiScene* scene, Context &context, int width, int height) {

	aiNode* cameraNode = scene->mRootNode->FindNode(scene->mCameras[0]->mName);
	aiCamera* camera = scene->mCameras[0];

	aiVector3D eye = camera->mPosition;
	aiVector3D lookAt = eye + camera->mLookAt;
	aiVector3D up = camera->mUp;

	Pinhole pinhole(toFloat3(eye), toFloat3(lookAt), normalize(toFloat3(up)), 2.0f);

	const char* path = "./Pinhole.ptx";
	Program program = context->createProgramFromPTXFile(path, "pinhole");
	context->setRayGenerationProgram(0, program);
	//TODO: Exception program 
	context->setExceptionEnabled(RT_EXCEPTION_ALL, 1);
	Program exception = context->createProgramFromPTXFile(path, "exception");
	context->setExceptionProgram(0, exception);
	context->setRayGenerationProgram(0, program);

	context["camera"]->setUserData(sizeof(Pinhole), &pinhole);

	Film film(width, height);
	Buffer output = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	output->setFormat(RT_FORMAT_FLOAT4);
	output->setSize(width, height);
	context["film"]->setUserData(sizeof(Film), &film);
	context["output"]->set(output);

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

void SceneBuilder::loadLightsForRender(const aiScene* scene,
	Context &context, vector<aiMesh*> &areaLights, vector<Light*> &lights) {

	unsigned int nLights = areaLights.size() + scene->mNumLights;

	Buffer lightBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
		RT_FORMAT_USER, nLights);
	lightBuffer->setElementSize(MAX_LIGHT_SIZE);
	Buffer lightPtrBuffer = context->createBuffer(RT_BUFFER_INPUT_OUTPUT,
		RT_FORMAT_USER, nLights);
	lightPtrBuffer->setElementSize(sizeof(void*));

	char* basePtr = (char*)lightBuffer->getDevicePointer(0);

	int lightIdx = 0;
	for (unsigned int i = 0; i < scene->mNumLights; i++) {
		aiLight* lightPtr = scene->mLights[i];
		if (lightPtr->mType == aiLightSource_POINT) {

			float3 position = toFloat3(lightPtr->mPosition);
			float3 color = toFloat3(lightPtr->mColorDiffuse);
			PointLight light(position, color);
			char* buffer = (char*)lightBuffer->map();
			memcpy(buffer + lightIdx*MAX_LIGHT_SIZE, &light, sizeof(light));
			lightBuffer->unmap();
			buffer = (char*)lightPtrBuffer->map();
			void* adress = basePtr + lightIdx*MAX_LIGHT_SIZE;
			memcpy(buffer + lightIdx*sizeof(void*), &adress, sizeof(void*));
			lightPtrBuffer->unmap();

		}
		lightIdx++;
		//[TODO] Directional light
		//[TODO] Spot light
	}

	for (unsigned int i = 0; i < areaLights.size(); i++) {

		aiMesh* mesh = areaLights[i];
		/*if (mesh->mNumFaces < 1 || mesh->mNumFaces > 2)
		{
		aiString name;
		m_scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_NAME, name);
		printf("Material %s: Does only support quadrangle light source NumFaces: %d.\n", name.C_Str(), mesh->mNumFaces);
		}*/
		aiString s = mesh->mName;
		aiFace face = mesh->mFaces[0];
		if (face.mNumIndices == 3) {
			float3 p0 = toFloat3(mesh->mVertices[face.mIndices[0]]);
			float3 p1 = toFloat3(mesh->mVertices[face.mIndices[1]]);
			float3 p2 = toFloat3(mesh->mVertices[face.mIndices[2]]);
			Parallelogram parallelogram(p0, p1, p2, true); //cuidado aqui
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			aiString name;
			material->Get(AI_MATKEY_NAME, name);
			aiColor3D color;
			material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			AreaLight light(parallelogram, toFloat3(color));
			char* buffer = (char*)lightBuffer->map();
			memcpy(buffer + lightIdx*MAX_LIGHT_SIZE, &light, sizeof(light));
			lightBuffer->unmap();
			buffer = (char*)lightPtrBuffer->map();
			void* adress = basePtr + lightIdx*MAX_LIGHT_SIZE;
			memcpy(buffer + lightIdx*sizeof(void*), &adress, sizeof(void*));
			lightPtrBuffer->unmap();
		}
		lightIdx++;
	}
	context["lights"]->setBuffer(lightPtrBuffer);

}