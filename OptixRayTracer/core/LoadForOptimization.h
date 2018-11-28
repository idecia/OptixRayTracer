#pragma once

Scene SceneBuilder::LoadForOptimization(Context context, const aiScene* scene) {

	context->setRayTypeCount(4);
	context->setEntryPointCount(2);
	context->setStackSize(2800);

	vector<Material> materials;
	loadMaterialsForOptimization(scene, context, materials);
	loadGeometryForOptimization(scene, context, materials);
	Scene optixScene;
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

	context->setMissProgram(RayType::REINHART_RADIANCE, missReinhart);
	context->setMissProgram(RayType::BECKERS_RADIANCE, missBeckers);

	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

		aiMaterial* material = scene->mMaterials[i];

		aiString name;
		material->Get(AI_MATKEY_NAME, name);

		if (IsMaterialEmissive(material))  {
			Material optixMaterial = context->createMaterial();
			optixMaterial->setClosestHitProgram(RayType::REINHART_RADIANCE, closestHitReinhart);
			optixMaterial->setAnyHitProgram(RayType::REINHART_SHADOW, anyHitReinhart);
			optixMaterial->setClosestHitProgram(RayType::BECKERS_RADIANCE, closestHitBeckers);
			optixMaterial->setAnyHitProgram(RayType::BECKERS_SHADOW, anyHitBeckers);
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
			optixMaterial->setClosestHitProgram(RayType::REINHART_RADIANCE, closestHitReinhart);
			optixMaterial->setAnyHitProgram(RayType::REINHART_SHADOW, anyHitReinhart);
			optixMaterial->setClosestHitProgram(RayType::BECKERS_RADIANCE, closestHitBeckers);
			optixMaterial->setAnyHitProgram(RayType::BECKERS_SHADOW, anyHitBeckers);
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

Group SceneBuilder::GetGroup(optix::Context &context, Material material, GeometryInstance geometry) {

	GeometryGroup geometryGroup = context->createGeometryGroup(instances.begin(), instances.end())
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
	Context &context, const vector<Material> &materials) {

	vector<Geometry> geometries;
	loadMeshesForOptimization(scene, context, geometries);

	Geometry city, building, window;
	Material cityM, buildingM, windowM;

	ClassifyGeometries(geometries, materials, city, building, window, cityM, buildingM, windowM);

	GeometryInstance cityInst = GetGeometryInstance(context,city, cityM);
	GeometryInstance buildingInst = GetGeometryInstance(context, building, buildingM);
	GeometryInstance windowInst = GetGeometryInstance(context, window, windowM);

	Group cityGroup = GetGroup(context, cityM, city);
	Group buildingGroup = GetGroup(context, buildingM, building);
	Group windowGroup = GetGroup(context, windowM, window);

	Group buildingWindowGroup = context->createGroup();
	buildingWindowGroup->setChildCount(2);
	buildingWindowGroup->setChild(0, buildingGroup);
	buildingWindowGroup->setChild(1, windowGroup);
	{
		optix::Acceleration acceleration = context->createAcceleration("NoAccel", "NoAccel");
		buildingWindowGroup->setAcceleration(acceleration);
	}

	Group root = context->createGroup();
	root->setChildCount(2);
	root->setChild(0, buildingWindowGroup);
	root->setChild(1, city);
	{
		optix::Acceleration acceleration = context->createAcceleration("Sbvh", "Bvh");
		buildingWindowGroup->setAcceleration(acceleration);
		root->setAcceleration(acceleration);
		acceleration->markDirty(); 
	}

	context["root"]->set(root); //ver si esto esta bien

}



void SceneBuilder::loadMeshesForOptimization(const aiScene* scene,
	Context &context, vector<Geometry> &geometries) {

	const char* path = "./Mesh.ptx";
	Program bboxProg = context->createProgramFromPTXFile(path, "boundingBox");
	Program intersecProg = context->createProgramFromPTXFile(path, "intersect");

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

		aiMesh* mesh = scene->mMeshes[i];
		mesh->mName;

		unsigned int numFaces = mesh->mNumFaces;
		unsigned int numVertices = mesh->mNumVertices;

		optix::Geometry geometry = context->createGeometry();
		geometries.push_back(geometry);
		geometry->setPrimitiveCount(numFaces);
		geometry->setIntersectionProgram(intersecProg);
		geometry->setBoundingBoxProgram(bboxProg);

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