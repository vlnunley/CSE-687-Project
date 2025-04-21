#include "Reduce.h"
#include <numeric>

using std::to_string;
using std::string;
using std::vector;
using std::accumulate;

Reduce::Reduce(FileManagement& fileManager)
	: fileManagement(fileManager) {
}

void Reduce::ReduceDown(const string& key, const vector<int>& values) {
	int sum = accumulate(values.begin(), values.end(), 0);
	Export(key, sum);
}

void Reduce::Export(const string& key, const int value) {
	string keysum = "(" + key + "," + to_string(value) + ")\n";
	fileManagement.writeToOutput("Output.txt", keysum);
}