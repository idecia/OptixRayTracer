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
	const vector<int>& GetIndices() const;


};

Face::Face() { }

Face::Face(int a, int b, int c) {

	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);

}

const vector<int>& Face::GetIndices() const {

	return indices;
}