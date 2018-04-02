#pragma once 

#include <iostream>
#include "core/Scene.h"
#include "shapes/parallelogram.h"
#include "lights/AllLights.h"
#include "3rdparty/assimp/Importer.hpp"
#include "3rdparty/assimp/scene.h"
#include "3rdparty/assimp/material.h"
#include "3rdparty/assimp/mesh.h"
#include "3rdparty/assimp/postprocess.h"
#include "core/optix_global.h"
#include "films/Film.h"
#include "cameras/Pinhole.h"
#include "brdfs/Lambertian.h"
#include "core/Ray.h"
#include <vector> 

using namespace std;

class SceneBuilder {

public:

	static Scene BuildFromFile(const string &filename);

private:

	static void loadMaterials(const aiScene* scene, 
		Context context, vector<Material> &materials);

	static bool ColorHasAnyComponent(const aiColor3D &color);

	static bool IsMaterialEmissive(const aiMaterial* material);

	static float3 toFloat3(aiVector3D  vector);
	static float3 toFloat3(aiColor3D  vector);

	static void ClassifyMeshes(const aiScene* scene,
		vector<aiMesh*> geometryMeshes, vector<aiMesh*> areaLightMeshes);

	static void loadLights(const aiScene* scene,
		Context context, vector<aiMesh*> areaLights, vector<Light*> lights);

	static void loadGeometry(const aiScene* scene, 
		Context context, const vector<aiMesh*> geometryMeshes, 
		const vector<Material> &materialsmaterials);

	static void loadMeshes(const aiScene* scene,
		Context context, const vector<aiMesh*> geometryMeshes,
		vector<Geometry> geometries);

	static GeometryInstance GetGeometryInstance(Context context,
		const Geometry &geometry, const Material &material);

	static Group SceneBuilder::GetGroupFromNode(optix::Context &context, const aiScene* scene,
		aiNode* node, const vector<Geometry> &geometries, const vector<Material> &materials);


	static void loadCamera(const aiScene* scene, Context context);


};

Scene SceneBuilder::BuildFromFile(const string &filename) {

	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, 
		aiPrimitiveType_POINT    | 
		aiPrimitiveType_LINE);

	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_Triangulate           |
		aiProcess_CalcTangentSpace      |
		aiProcess_FindInvalidData       |
		aiProcess_GenUVCoords           |
		aiProcess_TransformUVCoords     |
		//aiProcess_FindInstances       |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeGraph         |
		aiProcess_OptimizeMeshes        |
		aiProcess_PreTransformVertices  |
		aiProcess_GenSmoothNormals
		);
		
	if (scene == NULL) {
		string msg = "Assimp error while reading file: " + filename + "\n";
		throw  std::exception(msg.data);
	}

	Context context;

	vector<Material> materials;
	loadMaterials(scene, context, materials);
	
	vector<aiMesh*> geometryMeshes;
	vector<aiMesh*> areaLightMeshes;
	ClassifyMeshes(scene, geometryMeshes, areaLightMeshes);

	vector<Light*> lights;
	loadLights(scene, context, areaLightMeshes,lights);

	loadGeometry(scene, context, geometryMeshes, materials);
	//ver si es necesario calcular el ABBox de la escena

	loadCamera(scene, context);

	Scene scene(context);
	return scene;

		

}

bool SceneBuilder::ColorHasAnyComponent(const aiColor3D &color) {
	return color.r > 0.0f || color.g > 0.0f || color.b > 0.0f;
}

float3 SceneBuilder::toFloat3(aiColor3D vector)
{
	return make_float3(vector.r, vector.g, vector.b);
}

float3 SceneBuilder::toFloat3(aiVector3D  vector)
{
	return make_float3(vector.x, vector.y, vector.z);
}

bool SceneBuilder::IsMaterialEmissive(const aiMaterial* material) {

	aiColor3D emissivePower;
	bool isEmissive = material->Get(AI_MATKEY_COLOR_EMISSIVE, emissivePower) == AI_SUCCESS &&
		ColorHasAnyComponent(emissivePower);
	return isEmissive;

}

void SceneBuilder::ClassifyMeshes(const aiScene* scene,
	vector<aiMesh*> geometryMeshes, vector<aiMesh*> areaLightMeshes) {

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		unsigned int materialIdx = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[materialIdx];
		if (IsMaterialEmissive(material))
			areaLightMeshes.push_back(mesh);
		else
			geometryMeshes.push_back(mesh);
	}
}

void SceneBuilder::loadLights(const aiScene* scene,
	Context context, vector<aiMesh*> areaLights, vector<Light*> lights) {

	unsigned int nLights = areaLights.size + scene->mNumLights;

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
			float3 color    = toFloat3(lightPtr->mColorDiffuse);
			PointLight light(position, color);
			char* buffer = (char*)lightBuffer->map();
			memcpy(buffer + lightIdx*MAX_LIGHT_SIZE, &light, sizeof(light));
			lightBuffer->unmap();
			buffer = (char*)lightPtrBuffer->map();
			memcpy(buffer + lightIdx*sizeof(void*), basePtr + lightIdx*sizeof(void*), sizeof(void*));
			lightPtrBuffer->unmap();

		}
		lightIdx++;
		//[TODO] Directional light
		//[TODO] Spot light
	}

	for (unsigned int i = 0; i < areaLights.size; i++) {

		aiMesh* mesh = areaLights[i];
		/*if (mesh->mNumFaces < 1 || mesh->mNumFaces > 2)
		{
			aiString name;
			m_scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_NAME, name);
			printf("Material %s: Does only support quadrangle light source NumFaces: %d.\n", name.C_Str(), mesh->mNumFaces);
		}*/

		aiFace face = mesh->mFaces[0];
		if (face.mNumIndices == 3) {
			float3 p0 = toFloat3(mesh->mVertices[face.mIndices[0]]);
			float3 p1 = toFloat3(mesh->mVertices[face.mIndices[1]]);
			float3 p2 = toFloat3(mesh->mVertices[face.mIndices[2]]);
			Parallelogram parallelogram(p0, p1, p2, true);
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			aiColor3D color;
			material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			AreaLight light(parallelogram, toFloat3(color));
			char* buffer = (char*)lightBuffer->map();
			memcpy(buffer + lightIdx*MAX_LIGHT_SIZE, &light, sizeof(light));
			lightBuffer->unmap();
			buffer = (char*)lightPtrBuffer->map();
			memcpy(buffer + lightIdx*sizeof(void*), basePtr + lightIdx*sizeof(void*), sizeof(void*));
			lightPtrBuffer->unmap();
	  }
	  lightIdx++;
	}
	
}


void SceneBuilder::loadMaterials(const aiScene* scene,
	Context context, vector<Material> &materials) {

	const char* path = "./PathTracer.ptx";
	Program closestHitRadiance = context->createProgramFromPTXFile(path, "closestHit");
	Program anyHit = context->createProgramFromPTXFile(path, "anyHit");

	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {

		aiMaterial* material = scene->mMaterials[i];
		
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		
		if (IsMaterialEmissive(material)) 
			continue;

		//[TODO] Textured material
		//[TODO] Reflective/mirror material

		aiColor3D diffuseColor;
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {

			Material optixMaterial = context->createMaterial();
			Lambertian brdf(toFloat3(diffuseColor));
			optixMaterial["brdf"]->setUserData(sizeof(Lambertian), &brdf);
			materials.push_back(material);

		}

	}

}

void SceneBuilder::loadMeshes(const aiScene* scene,
	Context context, const vector<aiMesh*> geometryMeshes,
	vector<Geometry> geometries) {

	const char* path = "./Mesh.ptx";
	Program bboxProg = context->createProgramFromPTXFile(path, "boundingBox");
	Program intersecProg = context->createProgramFromPTXFile(path, "intersect");
	
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {

		aiMesh* mesh = scene->mMeshes[i];
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

Group SceneBuilder::GetGroupFromNode(optix::Context &context, const aiScene* scene,
	aiNode* node, const vector<Geometry> &geometries, const vector<Material> &materials) {

	if (node->mNumMeshes > 0) {

		vector<GeometryInstance> instances;
		GeometryGroup geometryGroup = context->createGeometryGroup();
		geometryGroup->setChildCount(node->mNumMeshes);
		
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			unsigned int meshIndex = node->mMeshes[i];
			aiMesh* mesh = scene->mMeshes[meshIndex];
			unsigned int materialIndex = mesh->mMaterialIndex;
			Material geometryMaterial = materials.at(materialIndex);
			GeometryInstance instance = GetGeometryInstance(context, geometries[meshIndex], geometryMaterial);
			geometryGroup->setChild(i, instance);
		}
		{
			Acceleration acceleration = context->createAcceleration("Sbvh", "Bvh");
			acceleration->setProperty("vertex_buffer_name", "vertexBuffer");
			acceleration->setProperty("index_buffer_name", "indexBuffer");
			geometryGroup->setAcceleration(acceleration);
			acceleration->markDirty();
		}

		// Create group that contains the GeometryInstance
		Group group = context->createGroup();
		group->setChildCount(1);
		group->setChild(0, geometryGroup);
		{
			optix::Acceleration acceleration = context->createAcceleration("NoAccel", "NoAccel");
			group->setAcceleration(acceleration);
		}

		return group;
	} else if (node->mNumChildren > 0) {
		
		vector<optix::Group> groups;
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			aiNode* childNode = node->mChildren[i];
			Group childGroup = GetGroupFromNode(context, scene, childNode, geometries, materials);
			if (childGroup) {
					groups.push_back(childGroup);
			}
		}
		if (groups.size() > 0) {
			Group group = context->createGroup(groups.begin(), groups.end());
			Acceleration acceleration = context->createAcceleration("Sbvh", "Bvh");
			group->setAcceleration(acceleration);
			return group;
		}
	}

	Group emptyGroup = context->createGroup();
	Acceleration acceleration = context->createAcceleration("NoAccel", "NoAccel");
	emptyGroup->setAcceleration(acceleration);
	return emptyGroup;
}

GeometryInstance SceneBuilder::GetGeometryInstance(Context context, const Geometry &geometry, const Material &material) {
	GeometryInstance instance = context->createGeometryInstance(geometry, &material, &material + 1);
	return instance;
}


void SceneBuilder::loadGeometry(const aiScene* scene,
	Context context, const vector<aiMesh*> geometryMeshes,
	const vector<Material> &materials) {

	vector<Geometry> geometries;
	loadMeshes(scene, context, geometryMeshes, geometries);

	Group rootNodeGroup = GetGroupFromNode(context, scene, scene->mRootNode, geometries, materials);

	Acceleration acceleration = context->createAcceleration("Sbvh", "Bvh");
	rootNodeGroup->setAcceleration(acceleration);
	acceleration->markDirty(); //_? Estudiar

	context["root"]->set(rootNodeGroup); //ver si esto esta bien

}

void SceneBuilder::loadCamera(const aiScene* scene, Context context) {

	aiNode* cameraNode = scene->mRootNode->FindNode(scene->mCameras[0]->mName);
	aiCamera* camera = scene->mCameras[0];

	aiVector3D eye = camera->mPosition;
	aiVector3D lookAt = eye + camera->mLookAt;
	aiVector3D up = camera->mUp;

	Pinhole camera(toFloat3(eye), toFloat3(lookAt), normalize(toFloat3(up)), 1.0f);

	const char* path = "./Pinhole.ptx";
	Program program = context->createProgramFromPTXFile(path, "pinhole");
	context->setRayGenerationProgram(0, program);
	path = "./Whitted.ptx";
	Program miss = context->createProgramFromPTXFile(path, "miss");
	context->setMissProgram(RayType::RADIANCE, miss);
	//TODO: Exception program 
	context["camera"]->setUserData(sizeof(Pinhole), camera);
	
	int width = 750;
	int height = 550;
	Film film(width, height);
	Buffer output = context->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	output->setFormat(RT_FORMAT_FLOAT4);
	output->setSize(width, height);
	context["film"]->setUserData(sizeof(Film), film);
	context["output"]->set(output);

}