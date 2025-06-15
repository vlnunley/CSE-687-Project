#include "Utils.h"
#include <sstream>


void Utils::sortWords(map<string, vector<int>>& map, string line) 
{
	char ch;
	string word;
	int wordCnt;

	std::stringstream ss(line);
	ss >> ch;
	std::getline(ss, word, ',');
	ss >> wordCnt;

	auto it = map.find(word);

	if (it != map.end()) {
		it->second.push_back(wordCnt);
	}
	else {
		vector<int > count{ wordCnt };
		map.insert(make_pair(word, count));
	}
};