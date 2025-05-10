#include "ReduceLibrary.h"
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier

using std::string;
using std::vector;
using std::accumulate;

int ReduceDown(const string& key, vector<int>& values) {
	int sum = accumulate(values.begin(), values.end(), 0);
	return sum;
}