//
#include "FileManagement.h"
#include "pch.h"

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

 void PrepairExport(pair<string, int> cpair);

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
		PrepairExport(currentPair);
	}

}

void PrepairExport(pair<string, int> cpair) {
	tempFileQueue.push(cpair);
}

void Export(FileManagement& _fileManager) {
	while (!tempFileQueue.empty()) {
		pair<string, int> outputline = tempFileQueue.front();
		string tempString = "(" + outputline.first + "," + std::to_string(outputline.second) + ")";
		_fileManager.writeToTemp(tempString);
		tempFileQueue.pop();
	}
}