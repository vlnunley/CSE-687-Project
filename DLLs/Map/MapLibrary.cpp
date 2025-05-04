//
//#include<string>
//#include <sstream>
//#include <vector>
//#include<iostream>
//#include <algorithm>
//#include<regex>
//#include<queue>
#include "FileManagement.h"
#include "MapLibrary.h"
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
//#include <utility>
//#include <limits.h>
//#include "MapLibrary.h"

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
using std::multimap;

FileManagement* fileManager;
queue<pair<string, int>> tempFileQueue;
multimap<string, int> words;
multimap<string, string>linesOfText;

 void Export(pair<string, int> cpair);


void Map(FileManagement& _fileManager) {
	fileManager = &_fileManager;
}


queue<pair<string, int>>  getTempFileQueue() {
	return tempFileQueue;
};

 void mapText(string key, string value) {

	string lineOfText = value;
	string sub = "";
	regex wordRegex("\\b[\\w'-]+\\b");
	smatch matches;
	linesOfText.insert(pair<string, string>(key, lineOfText));
	int count = 0;


	while (regex_search(lineOfText, matches, wordRegex)) {
		sub = matches[0].str();
		transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
		pair <string, int> currentPair = pair<string, int>(sub, 1);
		words.insert(currentPair);
		lineOfText = matches.suffix().str();
		Export(currentPair);
	}

}

void Export(pair<string, int> cpair) {
	tempFileQueue.push(cpair);

	if (tempFileQueue.size() == 100) {
		while (!tempFileQueue.empty()) {
			pair<string, int> outputline = tempFileQueue.front();
			string tempString = "(" + outputline.first + "," + std::to_string(outputline.second) + ")";
			fileManager->writeToTemp(tempString);
			tempFileQueue.pop();
		}
	}


}
void ExportRemaining() {
	while (!tempFileQueue.empty()) {
		pair<string, int> outputline = tempFileQueue.front();
		string tempString = "(" + outputline.first + "," + std::to_string(outputline.second) + ")";
		fileManager->writeToTemp(tempString);
		tempFileQueue.pop();
	}
}



void sortWords(map<string, vector<int>>& map, string line) {
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


int testingDLL() {
	return 4;
}
