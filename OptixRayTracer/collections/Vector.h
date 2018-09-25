#pragma once


template <typename T> 
void Append(vector<T> &v1, vector<T> &v2) {

	int n = v2.size();
	for (int i = 0; i < n; i++) {
		v1.push_back(v2[i]);
	}
}
