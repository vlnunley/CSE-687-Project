#include "Map.h"
#include<string>
#include <sstream>
#include <vector>
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
Map::Map() {

}
multimap<string, int>*Map::getMap() {
	return words;
}

multimap<string, string>* Map::getLinesOfText() {
	return linesOfText;
}

void Map::mapText(string key, string value) {
	
	string lineOfText = value;
	string sub = "";
	regex wordRegex("\\b[\\w'-]+\\b");
	smatch matches;
	linesOfText->insert(pair<string,string>(key, lineOfText));

	while (regex_search(lineOfText, matches, wordRegex)) {	
		sub = matches[0].str(); 
		transform(sub.begin(), sub.end(), sub.begin(),::tolower);
		words->insert(pair<string, int>(sub, 1));
		lineOfText = matches.suffix().str();  
		}	
}


