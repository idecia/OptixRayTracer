
#include <stdio.h>
#include "geometry/GeometricObject.h"
#include "geometry/Primitive.h"
#include "geometry/Mesh3D.h"
#include "math/Transform.h"
#include <iostream>
#include <fstream>

using namespace std;

void PrintVertex(ofstream &file, const float3 &v) {

	file << "v " << v.x << " "
		         << v.y << " " 
				 << v.z << "\n";

}

void PrintFace(ofstream &file, const Face* face, int offset) {

	const vector<int> ind = face->GetIndices();
	file << "f " << ind[0] + offset << " " 
		         << ind[1] + offset << " "
				 << ind[2] + offset << "\n";

}

void ExportToOBJ(ofstream &file, GeometricObject* obj, Transformation objToWorld, int &vertexOffset) {

	if (obj->IsPrimitive()) {
		const Mesh3D* mesh3D = ((Primitive*) obj)->Triangulate();
		const vector<float3> vertices = mesh3D->GetVertices();
		objToWorld = objToWorld * mesh3D->GetObjectToWorldTransformation();
		for (int i = 0; i < vertices.size(); i++) {
			const float3 v = objToWorld * vertices[i];
			PrintVertex(file, v);
		}
		file << "\n";
		const vector<Face*> faces = mesh3D->GetFaces();
		for (int i = 0; i < faces.size(); i++) { 
			PrintFace(file, faces[i], vertexOffset);
		}
		vertexOffset += vertices.size();
	}
	else {
		vector<GeometricObject*> childs = obj->GetChilds();
		for (int i = 0; i < childs.size(); i++) {
			GeometricObject* child = childs[i];
			Transformation objToWorldChild = obj->GetObjectToWorldTransformation();
			ExportToOBJ(file, child, objToWorld * objToWorldChild, vertexOffset);
		}
	}

}

void ExportModel(string filepath, GeometricObject* rootObject) {

	ofstream file;
	file.open(filepath);
	Transformation identity;
	int vertexOffset = 1;
	ExportToOBJ(file, rootObject, identity, vertexOffset);
	file.close();

	
}