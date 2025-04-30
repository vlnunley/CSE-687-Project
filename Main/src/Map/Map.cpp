#include "Map.h"
#include <sstream>
#include<iostream>
#include <algorithm>
#include<regex>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::transform;
using std::pair;
using std::regex;
using std::regex_search;
using std::smatch;
using std::queue;
using std::map;
Map::Map(FileManagement& fileManager) : fileManagement(fileManager) {

}


queue<pair<string, int>> Map::getTempFileQueue() {
	return tempFileQueue;
};

void Map::mapText(string key, string value) {
	
	string lineOfText = value;
	string sub = "";
	regex wordRegex("\\b[\\w'-]+\\b");
	smatch matches;
	linesOfText.insert(pair<string,string>(key, lineOfText));
	int count = 0;
	
	
	while (regex_search(lineOfText, matches, wordRegex)) {	
		sub = matches[0].str(); 
		transform(sub.begin(), sub.end(), sub.begin(),::tolower);
		pair <string,int> currentPair = pair<string, int>(sub, 1);
		words.insert(currentPair);
		lineOfText = matches.suffix().str();  
		Export(currentPair);
	}
	
}

void Map::Export(pair<string,int> cpair) {
	tempFileQueue.push(cpair);

	if (tempFileQueue.size() == 100) {
		while (!tempFileQueue.empty()) {
			pair<string, int> outputline = tempFileQueue.front();
			string tempString = "(" + outputline.first + "," + std::to_string(outputline.second) + ")\n";
			fileManagement.writeToTemp(tempString);
			tempFileQueue.pop();
		}
	}
}
void Map::Export() {
	while (!tempFileQueue.empty()) {
		pair<string, int> outputline = tempFileQueue.front();
		string tempString = "(" + outputline.first + "," + std::to_string(outputline.second) + ")\n";
		fileManagement.writeToTemp(tempString);
		tempFileQueue.pop();
	}
}

void Map::sortWords(map<string, vector<int>>& map,string line) {
	int endOfNumber = line.find_first_of(',');
	string parsed = line.substr(1, endOfNumber - 1);
	auto it = map.find(parsed);

	if (it != map.end()) {
		it->second.push_back(1);
	}
	else {
		vector<int > count{ 1 };
		map.insert(make_pair(parsed, count));
	}
};

