#pragma once
#include "core/optix_global.h"
#include <chrono>
#include "geometry/Mesh3D.h"
#include "blinds/RectangularBlind.h"
using namespace std;
using namespace std::chrono;


class Scene {

public:
	RT_FUNCTION Scene();
	RT_FUNCTION Scene(Context context);
	RT_FUNCTION void Render();
	RT_FUNCTION Buffer GetOutputImage();
	RT_FUNCTION Buffer GetSensorValues();
	RT_FUNCTION void SetWidth(int width);
	RT_FUNCTION void SetHeight(int height);
	RT_FUNCTION int GetWidth();
	RT_FUNCTION int GetHeight();
	RT_FUNCTION void ChangeSensorPosition(float3 position);
	RT_FUNCTION void ResetSensorValues();
	RT_FUNCTION Buffer GetEnvValues();
	RT_FUNCTION void ComputeEnv();
	RT_FUNCTION void ComputeDayLight();
	RT_FUNCTION void ComputeDCSensors();
	RT_FUNCTION void Optimize();
	RT_FUNCTION void ComputeSensorPositionsAndNormals();
	RT_FUNCTION void ComputeDCSensor(int i, float3 position, float3 normal);
	RT_FUNCTION void ComputeSkyValues();
	RT_FUNCTION void LoadBlindToOptix(Mesh3D* mesh);

	Context context;
	int width;
	int nSamples;
	int height;
	float3 sensorPos;
	float3 sensorNormal;
	static const unsigned int NskyPatches = 146;
	static const unsigned int NEnvironmentalPatches = 289;
	static const unsigned int Nsensors = 72;
	static const unsigned int HoursPerYear = 30;
	vector<float3> sensorPositions;
	vector<float3> sensorNormals;
	vector<Acceleration> accelerations;
	Matrix<NEnvironmentalPatches, NskyPatches> ENV;
	Matrix<Nsensors, NEnvironmentalPatches> SENS;
	Matrix<NskyPatches, HoursPerYear> SKYES ;
	Matrix<Nsensors, HoursPerYear> E ;
	Buffer coeff;
	Buffer env;
	Geometry blindGeometry; 
	GeometryInstance blindGeometryInstance; 
	GeometryGroup blindGroup;
	Group root;
	Group buildingWindowGroup;


private:

};

RT_FUNCTION  Scene::Scene() {

}

RT_FUNCTION  Scene::Scene(Context context) 
	: context(context) {
}

RT_FUNCTION int Scene::GetWidth() {
	return width;
}

RT_FUNCTION int Scene::GetHeight() {
	return height;
}


RT_FUNCTION void Scene::SetWidth(int width) {
	this->width = width;
}

RT_FUNCTION void Scene::SetHeight(int height) {
	this->height = height;
}


RT_FUNCTION void Scene::Render() {

	context->setPrintEnabled(true);
	context->validate();
	context->compile();
	if (height == 0)
		context->launch(0, width);
	else
		context->launch(0, width, height);

}


RT_FUNCTION void Scene::ComputeEnv() {

	context["sensorPos"]->setFloat(sensorPos);
	context["sensorNormal"]->setFloat(sensorNormal);
	context->setPrintEnabled(true);
	context->validate();
	context->compile();
    context->launch(0, width);

	Buffer coeff = GetEnvValues();
	float3* values = (float3*)coeff->map();
	RTsize RTwidth; RTsize RTheight;
	coeff->getSize(RTwidth, RTheight);
	int width = static_cast<int>(RTwidth);
	int height = static_cast<int>(RTheight);
	for (int i = 0; i < width; i++)  {
		for (int j = 0; j < height; j++) {
			float3 v = values[j*width + i];
			ENV[i*height + j] = v.x;
		}
	}
	coeff->unmap();

	
}

RT_FUNCTION void Scene::ComputeDCSensors() {

		context->validate();
		context->compile();
		context->launch(1, 0);

	
}

RT_FUNCTION void Scene::ComputeDCSensor(int i,  float3 position, float3 normal) {

	ResetSensorValues();
	context["sensorPos"]->setFloat(position);
	context["sensorNormal"]->setFloat(normal);
	context->setPrintEnabled(true);
	context->validate();
	context->compile();
	context->launch(1, width);
	Buffer coeff = GetSensorValues();
	float3* values = (float3*)coeff->map();
	RTsize RTwidth; RTsize RTheight;
	coeff->getSize(RTwidth, RTheight);
	int width = static_cast<int>(RTwidth);
	for (int j = 0; j < width; j++) {
		float3 v = values[j];
		SENS[i*width + j] = v.x;
	}
	coeff->unmap();
}

RT_FUNCTION void Scene::ComputeSensorPositionsAndNormals() {

	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 12; j++) {
			float3 p = make_float3(-1.25 + i*0.5, -0.5 - j*0.5, 0.7);
			sensorPositions.push_back(p);
			sensorNormals.push_back(make_float3(0.0f, 0.0f, 1.0f));
		}
}

RT_FUNCTION void Scene::ComputeSkyValues() {

	FILE * pFile;
	pFile = fopen("URY_Montevideo.mtx", "r");
	float3 p;
	for (int i = 0; i < NskyPatches; i++) {
		for (int j = 0; j < HoursPerYear; j++) {
			fscanf(pFile, "%f\t%f\t%f\n", &p.x, &p.y, &p.z);
			SKYES[i*HoursPerYear + j] = p.x;
		}
	}
	fclose(pFile);
}


RT_FUNCTION void Scene::LoadBlindToOptix(Mesh3D* mesh) {

	Buffer vertexBuffer = blindGeometry["vertexBuffer"]->getBuffer();
	Buffer indexBuffer = blindGeometry["indexBuffer"]->getBuffer();
	float3* vertexBuffer_Host = static_cast<optix::float3*>(vertexBuffer->map());
	optix::int3* indexBuffer_Host = static_cast<optix::int3*>(indexBuffer->map());

	const vector<float3> & vertices = mesh->GetVertices();
	memcpy(static_cast<void*>(vertexBuffer_Host), (&vertices[0]), sizeof(optix::float3)*vertices.size());
	vertexBuffer->unmap();

	// Copy index buffer from host to device
	const vector<Face*> & faces = mesh->GetFaces();
	for (unsigned int i = 0; i < faces.size(); i++) {
		const vector<int> & indices = faces[i]->GetIndices();
		indexBuffer_Host[i].x = indices[0];
		indexBuffer_Host[i].y = indices[1];
		indexBuffer_Host[i].z = indices[2];
	}
	indexBuffer->unmap();

	blindGeometry->setPrimitiveCount(faces.size());
	blindGeometry->markDirty();
	
	blindGroup->getAcceleration()->markDirty();
	buildingWindowGroup->getAcceleration()->markDirty();

}

RT_FUNCTION void Scene::Optimize() {

	ComputeSkyValues();
	ComputeSensorPositionsAndNormals();
	ComputeEnv();

	ComputeDCSensors(); //DUMMY FOR OPTIX INITIALIZATION
	//for (int i = 0; i < accelerations.size(); i++)
		//(accelerations[i])->markDirty();
	//ComputeDCSensors();
	//accelerations[1]->markDirty();
	//accelerations[0]->markDirty();
	//ComputeDCSensors();
	//accelerations[accelerations.size() - 1]->markDirty();


	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	float w = 0.15f, h = 0.01f, l = 2.0f, a = 0.7f, p = 0.0f; int n = 10;
	RectangularBlind Blind(w, h, l, a, p, n);
	Mesh3D* mesh = Blind.GetMesh();
	LoadBlindToOptix(mesh);
	delete mesh;

	for (int i = 0; i < sensorPositions.size(); i++) {

		ComputeDCSensor(i, sensorPositions[i], sensorNormals[i]);
	}

	Matrix<Nsensors, NskyPatches> DC = SENS*ENV;
	E = DC*SKYES;

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	std::chrono::duration<double> elapsed = t2 - t1;
	cout << elapsed.count() << "\n";


	
	/*for (int i = 0; i < NEnvironmentalPatches; i++) {
		for (int j = 0; j < NskyPatches; j++) {
			float v = ENV[i*NskyPatches + j];
			cout << "   " << v;
			}
		cout << "\n";
	}*/

	/*for (int i = 0; i < Nsensors; i++) {
		for (int j = 0; j < NEnvironmentalPatches; j++) {
			float v = SENS[i*NEnvironmentalPatches + j];
			cout << "   " << v;
		}
	cout << "\n";
	}*/

	/*
	for (int i = 0; i < Nsensors; i++) {
		for (int j = 0; j < NskyPatches; j++) {
			float v = DC[i*NskyPatches + j];
			cout << "   " << v;
		}
		cout << "\n";
	}*/

	for (int i = 0; i < Nsensors; i++) {
		for (int j = 0; j < HoursPerYear; j++) {
			float v = E[i*HoursPerYear + j];
			cout << "   " << v;
		}
		cout << "\n";
	}

}



RT_FUNCTION Buffer Scene::GetOutputImage() {

	return context["output"]->getBuffer();
}

RT_FUNCTION Buffer Scene::GetSensorValues() {

	return context["coeff"]->getBuffer();
}

RT_FUNCTION Buffer Scene::GetEnvValues() {

	return context["env"]->getBuffer();
}


RT_FUNCTION void Scene::ChangeSensorPosition(float3 position) {

	context["sensorPos"]->setFloat(position);

}

RT_FUNCTION void Scene::ResetSensorValues() {

	float* values = (float*)coeff->map();
	for (unsigned int i = 0; i < NEnvironmentalPatches * 3; i++) {
		values[i] = 0.0f;
	}
	coeff->unmap();
}
