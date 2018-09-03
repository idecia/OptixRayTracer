#pragma once

#include "core/optix_global.h"
#include <vector>

using namespace std;

class Face {

private:

	vector<int> indices;

public:

	Face();
	Face(int a, int b, int c);
	Face(int a, int b, int c, int d);
	const vector<int>& GetIndices() const;
	void AddIndex(int i);


};

Face::Face() { }

Face::Face(int a, int b, int c) {

	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);

}

Face::Face(int a, int b, int c, int d) {

	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
	indices.push_back(d);

}

void Face::AddIndex(int i) {

	indices.push_back(i);

}

const vector<int>& Face::GetIndices() const {

	return indices;
}