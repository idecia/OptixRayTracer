#pragma once
#include "core/optix_global.h"
#include <chrono>
#include "geometry/Mesh3D.h"
#include "blinds/RectangularBlind.h"
#include <Eigen/Dense>
#include "3rdparty/pcg32-master/pcg32.h"
#include <limits>
using namespace std;
using namespace std::chrono;
using namespace Eigen;


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
	RT_FUNCTION int NeighborhoodSize(int k);
	RT_FUNCTION void NewNeighbor(float x[], int k, float xnew[]);
	RT_FUNCTION float ComputeFitness(float xnew[], Eigen::Matrix<float, Dynamic, Dynamic> E);
	RT_FUNCTION void UpdateSolution(float x[], float xnew[]);
	RT_FUNCTION void GetPermutation(int perm[], int n);
	RT_FUNCTION void EvaluateSensors(float x[]);
	RT_FUNCTION void ComputeDCBruteForce();

	Context context;
	int width;
	int nSamples;
	int height;
	float3 sensorPos;
	float3 sensorNormal;
	static const unsigned int NskyPatches = 146;
	static const unsigned int NEnvironmentalPatches = 146;
	static const unsigned int Nsensors = 72;
	static const unsigned int HoursPerYear = 3650;
	vector<float3> sensorPositions;
	vector<float3> sensorNormals;
	vector<Acceleration> accelerations;
	//optix::Matrix<NEnvironmentalPatches, NskyPatches-1> ENV;
	//optix::Matrix<Nsensors, NEnvironmentalPatches> SENS;
	//optix::Matrix<NskyPatches - 1, HoursPerYear> SKYES;
	//optix::Matrix<Nsensors, HoursPerYear> E;
	Buffer coeff;
	Buffer env;
	Geometry blindGeometry;
	GeometryInstance blindGeometryInstance;
	GeometryGroup blindGroup;
	Group root;
	Group buildingWindowGroup;
	Eigen::Matrix<float, Dynamic, Dynamic> ENV;
	Eigen::Matrix<float, Dynamic, Dynamic> SENS;
	Eigen::Matrix<float, Dynamic, Dynamic> SKYES;
	Eigen::Matrix<float, Dynamic, Dynamic> DC;
	Eigen::Matrix<float, Dynamic, Dynamic> E;

	pcg32 rng;

	static const int maxFuncEval = 500;
	static const int minNumNeigh = 25;
	static const int numOfVars = 4;
	static const int nsteps = 2;
	static const int ndeltas = 5;
	static const int Kmax = nsteps*ndeltas;

	static const float xM[numOfVars];
	static const float xm[numOfVars];
	static const  int stepvalues[nsteps];
	static const float deltavalues[ndeltas];
	static const float UDI[2];
	int nextstep[Kmax];
	int nextdelta[Kmax];


private:

};

//RectangularBlind::RectangularBlind(float w, float t, float l, float a, float s, int N, float c, float H, float W, float L)
//RectangularBlind Blind(0.4, 0.001, 2.0, 2, 0.3, 4, 0.05, 1.5, 2, 0.25);
//                                  w,     a,     s,     c
const float Scene::xM[numOfVars] = { 0.25, M_PIf, 0.5 , 0.125 };
const float Scene::xm[numOfVars] = { 0.0f, 0.0f , 0.0f, 0.0f  };
const  int Scene::stepvalues[nsteps] = { 1, 2 };
const float Scene::deltavalues[ndeltas] = { 0.1f, 0.15f, 0.3f, 0.5f, 0.8f };
//const float Scene::UDI[2] = { 0.001f, 0.5f };
//const float Scene::UDI[2] = { 0.08f, 1000.0f };
const float Scene::UDI[2] = { 0.55f, 16.76f };
//const float Scene::UDI[2] = { 0.558f, 16.75f };

RT_FUNCTION  Scene::Scene() {

}

RT_FUNCTION  Scene::Scene(Context context)
	: context(context),
	ENV(NEnvironmentalPatches, NskyPatches - 1),
	SENS(Nsensors, NEnvironmentalPatches),
	SKYES(NskyPatches - 1, HoursPerYear),
	DC(Nsensors, NskyPatches - 1),
	E(Nsensors, HoursPerYear) {

	rng.seed(42u, 54u);


	//cout << UDI[0] << "    " << UDI[1] << endl;

	int z = 0;
	for (int k = 0; k <= (nsteps - 1) + (ndeltas - 1); k++) {
		for (int j = k; j >= 0; j--) {
			int i = k - j;
			if ((j < nsteps) && (i < ndeltas)) {
				nextstep[z] = j;
				nextdelta[z] = i;
				z++;
			}
		}
	}
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

	context["sensorPos"]->setFloat(sensorPos + make_float3(0.1, 0.1, 0.1));
	context["sensorNormal"]->setFloat(sensorNormal);
	context->setPrintEnabled(true);
	context->validate();
	context->compile();
	context->launch(0, width);


	Buffer coeff2 = context["coeff2"]->getBuffer();
	int* valcoeff2 = (int*)coeff2->map();

	Buffer coeff = GetEnvValues();
	//float3* values = (float3*)coeff->map();
	float* values = (float*)coeff->map();
	RTsize RTwidth; RTsize RTheight;
	coeff->getSize(RTwidth, RTheight);
	int width = static_cast<int>(RTwidth);
	int height = static_cast<int>(RTheight);
	for (int i = 0; i < width; i++)  {

		for (int j = 1; j < height; j++) {
			//float3 v = values[j*width + i];
			float v = values[j*width + i];
			//ENV[i*(height-1) + j-1] = v.x;
			//ENV(i, j - 1) = v.x
			ENV(i, j - 1) = v;
			int n = valcoeff2[i];
			if (n != 0)
				ENV(i, j - 1) = v / n;
			//cout << "   " << ENV(i, j - 1);
		}
		//cout << endl;
	}
	coeff->unmap();

	coeff2->unmap();
}

RT_FUNCTION void Scene::ComputeDCSensors() {

	context->validate();
	context->compile();
	context->launch(1, 0);


}

RT_FUNCTION void Scene::ComputeDCBruteForce() {
	context->setPrintEnabled(true);
	context->validate();
	context->compile();
	context->launch(0, 0);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 12; j++)  {
			float3 p = make_float3(-1.25 + i*0.5, -0.5 - j*0.5, 27.7);
			//float3 p = make_float3(1.0f - i*1.0, -1.25f - j*2.0f, 0.7f);
			ResetSensorValues();
			context["sensorPos"]->setFloat(p);
			context["sensorNormal"]->setFloat(make_float3(0.0f, 0.0f, 1.0f));
			context->launch(0, width);
			Buffer coeff = GetSensorValues();
			float* values = (float*)coeff->map();
			//float3* values = (float3*)coeff->map();
			RTsize RTwidth; RTsize RTheight;
			coeff->getSize(RTwidth, RTheight);
			int width = static_cast<int>(RTwidth);
			for (int j = 0; j < width; j++) {
				//float3 v = values[j];
				float v = values[j];
				//cout << v.x << "   ";
				cout << v << "   ";
			}
			cout << endl;
			coeff->unmap();
		}
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	std::chrono::duration<double> elapsed = t2 - t1;
	//cout << elapsed.count() << "\n";

}

RT_FUNCTION void Scene::ComputeDCSensor(int i, float3 position, float3 normal) {

	ResetSensorValues();
	context["sensorPos"]->setFloat(position);
	context["sensorNormal"]->setFloat(normal);
	context->setPrintEnabled(true);
	context->validate();
	context->compile();
	context->launch(1, width);
	Buffer coeff = GetSensorValues();
	//float3* values = (float3*)coeff->map();
	float* values = (float*)coeff->map();
	RTsize RTwidth; RTsize RTheight;
	coeff->getSize(RTwidth, RTheight);
	int width = static_cast<int>(RTwidth);
	for (int j = 0; j < width; j++) {
		//float3 v = values[j];
		float v = values[j];
		//SENS[i*width + j] = v.x;
		//cout << i << "   " << v.x << endl;
		//SENS(i,j) = v.x;
		SENS(i, j) = v;
	}
	coeff->unmap();
}

RT_FUNCTION void Scene::ComputeSensorPositionsAndNormals() {

	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 12; j++) {
			float3 p = make_float3(-1.25 + i*0.5, -0.5 - j*0.5, 27.7);
			sensorPositions.push_back(p);
			sensorNormals.push_back(make_float3(0.0f, 0.0f, 1.0f));
		}
	/*for (int i = 0; i < 3; i++)
	for (int j = 0; j < 3; j++) {
	float3 p = make_float3(1.0f -i*1.0, -1.25f -j*2.0f, 0.7f);
	sensorPositions.push_back(p);
	sensorNormals.push_back(make_float3(0.0f, 0.0f, 1.0f));
	}*/

}

RT_FUNCTION void Scene::ComputeSkyValues() {

	FILE * pFile;
	pFile = fopen("Sky145Montevideo8_17.txt", "r");

	float3 p;
	for (int i = 0; i < NskyPatches; i++) {
		for (int j = 0; j < HoursPerYear; j++) {
			fscanf(pFile, "%f\n", &p.x);
			if (i != 0) //salteamos los datos del parche 0 del cielo
				//SKYES[(i-1)*HoursPerYear + j] = p.x;
				SKYES(i - 1, j) = p.x;
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

RT_FUNCTION int Scene::NeighborhoodSize(int k) {

	int num = stepvalues[k] * (deltavalues[k] / deltavalues[0]);
	return minNumNeigh*num;

}

RT_FUNCTION void Scene::GetPermutation(int perm[], int n) {

	for (int i = 0; i < n; i++) {
		perm[i] = i;
	}
	rng.shuffle(perm, perm + n);

}

RT_FUNCTION void Scene::NewNeighbor(float x[], int k, float xnew[]) {

	int step = stepvalues[nextstep[k]];
	float delta = deltavalues[nextdelta[k]];

	int perm[numOfVars];
	GetPermutation(perm, numOfVars);

	for (int i = 0; i < step; i++) {
		int v = perm[i];
		float u = (2.0f*rng.nextFloat() - 1.0f);
		float rad = delta*(xM[v] - xm[v]);
		float val = x[v] + u*rad;
		if (val > xM[v])
			val = xM[v];
		if (val < xm[v])
			val = xm[v];
		xnew[v] = val;
	}

}

RT_FUNCTION float Scene::ComputeFitness(float xnew[], Eigen::Matrix<float, Dynamic, Dynamic> E) {

	int udi = 0;
	for (int j = 0; j < HoursPerYear; j++) {
		int ok = 1;
		for (int i = 0; i < Nsensors; i++) {
			if ((E(i, j) < UDI[0]) || (E(i, j) > UDI[1])) {
				ok = 0;
				break;
			}
		}
		udi += ok;
	}
	return ((float)udi) / HoursPerYear;
	//return -abs((((float)udi) / HoursPerYear) - 0.333);
}

RT_FUNCTION void Scene::UpdateSolution(float x[], float xnew[]) {

	for (int i = 0; i < numOfVars; i++) {
		x[i] = xnew[i];
	}

}

RT_FUNCTION void Scene::EvaluateSensors(float x[]) {


	RectangularBlind Blind(1, 0.01, 2.0, 0.707, 0.4, 4, 0.0, 1.5, 2, 0.25);
	Blind.RepairBlind();
	Mesh3D* mesh = Blind.GetMesh();
	LoadBlindToOptix(mesh);
	delete mesh;
	for (int i = 0; i < Nsensors; i++) {
		ComputeDCSensor(i, sensorPositions[i], sensorNormals[i]);
	}
	//optix::Matrix<Nsensors, NskyPatches-1> DC = SENS*ENV;
	/*for (int i = 0; i < Nsensors; i++) {
		for (int j = 0; j < NEnvironmentalPatches; j++) {
		//float v = DC[i*(NskyPatches-1) + j];
		float v = SENS(i, j);
	//	cout << "   " << v;
		}
	//cout << "\n";
	}*/
	DC = SENS*ENV;
	E = DC*SKYES;
	/*for (int i = 0; i < Nsensors; i++) {
		for (int j = 0; j < NskyPatches - 1; j++) {
			//float v = DC[i*(NskyPatches-1) + j];
			float v = DC(i, j);
			cout << "   " << v;
		}
		cout << "\n";
	}
	cout << "\n\n";*/

}

//RT_FUNCTION void Scene::LocalSearch(float[])


RT_FUNCTION void Scene::Optimize() {

	ComputeSkyValues();
	ComputeSensorPositionsAndNormals();

	context->launch(0, 0);
	//high_resolution_clock::time_point t1 = high_resolution_clock::now();
	ComputeEnv();
	//high_resolution_clock::time_point t2 = high_resolution_clock::now();
	//std::chrono::duration<double> elapsed = t2 - t1;
	//cout << elapsed.count() << "\n";



	ComputeDCSensors(); //DUMMY FOR OPTIX INITIALIZATION
	//for (int i = 0; i < accelerations.size(); i++)
	//(accelerations[i])->markDirty();
	//ComputeDCSensors();
	//accelerations[1]->markDirty();
	//accelerations[0]->markDirty();
	//ComputeDCSensors();
	//accelerations[accelerations.size() - 1]->markDirty();
	//const float Scene::xM[numOfVars] = { M_PIf / 2.0f, 0.15f, 0.02f, 2.0f, 0.1f };
	//const float Scene::xm[numOfVars] = { 0.0f, 0.05f, 0.01f, 1.0f, 0.0f };

	//initial solution
	//float x[numOfVars] = { M_PIf / 2.0f, 0.08, 0.01, 1.0, 0.05f  };
	//float x[numOfVars] = { M_PIf / 2.0f, 0.15, 0.01, 0.0 };

	float x[numOfVars] = { 0.668928, 0.10603 };
	float fitness = 0.0f;
	EvaluateSensors(x);/*
	fitness = ComputeFitness(x, E);
	cout << "INI:   " << fitness << endl;
	int numFuncEval = 0;

	while (numFuncEval < maxFuncEval) {

	for (int k = 0; (k < Kmax) && (numFuncEval < maxFuncEval); k++) {

	int ptries = NeighborhoodSize(k);
	for (int p = 0; (p < ptries) && (numFuncEval < maxFuncEval); p++) {

	float xnew[numOfVars];
	xnew[0] = x[0];
	xnew[1] = x[1];
	//xnew[2] = x[2];
	//xnew[3] = x[3];
	//xnew[4] = x[4];
	NewNeighbor(x, k, xnew);

	EvaluateSensors(xnew);
	numFuncEval++;
	float newFitness = ComputeFitness(xnew, E);
	if (newFitness > fitness) {
	EvaluateSensors(xnew);
	float newFitness2 = ComputeFitness(xnew, E);
	if (newFitness2 > fitness) {
	//cout << newFitness << "  " << newFitness2 << "       " << x[0] << "  " << x[1] << "  " << x[2] << "  " << x[3] << "      " << numFuncEval << endl;
	cout << newFitness << "  " << newFitness2 << "       " << x[0] << "      " << x[1] << "      " << numFuncEval << endl;
	newFitness = newFitness < newFitness2 ? newFitness : newFitness2;
	UpdateSolution(x, xnew);
	fitness = newFitness;
	//cout << newFitness << "  " << newFitness2 << "       " << x[0] << "  " << x[1] << endl;
	//cout << newFitness << "  " << newFitness2 << "       " << x[0] << "  "  << endl;
	k = 0;
	break;
	}
	}
	}

	}

	}

	EvaluateSensors(x);
	fitness = ComputeFitness(x, E);
	cout << "FINAL:   " << fitness << endl;

	//high_resolution_clock::time_point t2 = high_resolution_clock::now();
	//	std::chrono::duration<double> elapsed = t2 - t1;
	cout << elapsed.count() << "\n";*/



	/*
	for (int i = 0; i < NEnvironmentalPatches; i++) {
	for (int j = 0; j < NskyPatches-1; j++) {
	//float v = ENV[i*(NskyPatches-1) + j];
	float v = ENV(i, j);
	cout << "   " << v;
	}
	cout << "\n";
	}
	cout << "\n\n";
	for (int i = 0; i < Nsensors; i++) {
	for (int j = 0; j < NEnvironmentalPatches; j++) {
	//float v = SENS[i*NEnvironmentalPatches + j];
	float v = SENS(i,j);
	cout << "   " << v;
	}
	cout << "\n";
	}
	cout << "\n\n";

	for (int i = 0; i < Nsensors; i++) {
	for (int j = 0; j < NskyPatches-1; j++) {
	//float v = DC[i*(NskyPatches-1) + j];
	float v = DC(i, j);
	cout << "   " << v;
	}
	cout << "\n";
	}
	cout << "\n\n";

	for (int i = 0; i < Nsensors; i++) {
	for (int j = 0; j < HoursPerYear; j++) {
	//float v = E[i*HoursPerYear + j];
	float v = E(i, j);
	cout << "   " << v;
	}
	cout << "\n";
	}*/


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
	RTsize RTwidth; RTsize RTheight;
	coeff->getSize(RTwidth, RTheight);
	int width = static_cast<int>(RTwidth);
	for (unsigned int i = 0; i < width; i++) {
		values[i] = 0.0f;
	}
	coeff->unmap();
}
