#include "FileManagement.h"
#include "ReduceLibrary.h"
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier

using std::to_string;
using std::string;
using std::vector;
using std::accumulate;

FileManagement* fileManager;
void Export(const string& key, const int value);

void Reduce(FileManagement& fileManager)
	: fileManagement(fileManager) {
}

void ReduceDown(const string& key, const vector<int>& values) {
	int sum = accumulate(values.begin(), values.end(), 0);
	Export(key, sum);
}

void Export(const string& key, const int value) {
	string keysum = "(" + key + "," + to_string(value) + ")\n";
	fileManagement.writeToOutput("Output.txt", keysum);
}

int testingDLL() {
	return 4;
}
