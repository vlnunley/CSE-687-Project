#pragma once
#include<string>
#include<map>
#include <vector>
#include "../FileManagement/FileManagement.h"
#include<queue>

using std::multimap;
using std::string;
using std::vector;
using std::queue;
using std::pair;
using std::map;

class Map
{
	multimap<string,int> words;
	multimap<string, string>linesOfText;
	FileManagement& fileManagement;
	queue<pair<string,int>> tempFileQueue;
	void Export(pair<string,int>);
public:
	Map(FileManagement& fileManager);
	queue<pair<string, int>> getTempFileQueue();
	void mapText(string key,string value);
	void sortWords(map<string, vector<int>>& map, string line);
	void Export();
};

