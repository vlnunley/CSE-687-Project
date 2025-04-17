#include "Reduce.h"
#include<string>

Reduce::Reduce() {

}

void Reduce::reduce(const std::string& key, const std::vector<int>& values) {
	int sum = std::accumulate(values.begin(), values.end(), 0);
	export(key, sum);
}

void Reduce::export(const std::string& key, const int value) {
	// TODO - write result out to the output directory
	// TODO - write SUCCESS to the output directory
}