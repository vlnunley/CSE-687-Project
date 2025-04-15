#pragma once
#include<string>
#include<map>
#include <vector>
using std::multimap;
using std::string;
using std::vector;
class Map
{
	multimap<string,int>* words = new multimap<string,int>();
	multimap<string, string>* linesOfText = new multimap<string,string>;
public:
	Map();
	void mapText(string key,string value);

	multimap<string,int>*getMap();
	multimap<string,string>* getLinesOfText();
};

