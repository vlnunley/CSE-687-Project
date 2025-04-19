#include "Reduce.h"
#include<string>
#include "../FileManagement/FileManagement.h"

Reduce::Reduce(FileManagement& fileManager)
: fileManagement(fileManager) {
}

void Reduce::reduce(const std::string& key, const std::vector<int>& values) {
	int sum = std::accumulate(values.begin(), values.end(), 0);
	export(key, sum);
}

void Reduce::export(const std::string& key, const int value) {
	fileManagement.writeToOutput(key, std::to_string(value));
	fileManagement.writeToOutput("SUCCESS", "");
}