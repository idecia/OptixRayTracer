#pragma once

#include "core/optix_global.h"
#include <vector>

using namespace std;

class Face {

private:

	vector<int> indices;

public:

	const vector<int>& GetIndices() const;


};

const vector<int>& Face::GetIndices() const {

	return indices;
}