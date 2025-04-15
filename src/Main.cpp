
#include "FileManagement/FileManagement.h"
#include <iostream>
#include<string>
#include "Map/Map.h";
#include<map>
#include <Windows.h>
#include<vector>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::vector;
using std::ofstream;
using std::map;
using std::multimap;
using std::pair;

int main()
{
	FileManagement fileManage;
	Map* mapFile = new Map();
	string inpFileDirectory;
	string outpFileDirectory;
	string temFileDirectory;

	//grab input directory from the user.
	cout << "Please enter the directory that holds the input files: ";
	cin >> inpFileDirectory;
	fileManage.setInputFileDirectory(inpFileDirectory);
	//grab the short name for the file.
	fileManage.parseShortFileName(inpFileDirectory);
	
	//grab the output directory from the user.
	cout << "Please enter the directory that holds the output files: ";
	cin >> outpFileDirectory;
	fileManage.setOutputFileDirectory(outpFileDirectory);
	//grab the temporary directory from the user.
	cout << "Please enter the directory that holds the temp files: ";
	cin >> temFileDirectory;
	fileManage.setTempFileDirectory(temFileDirectory);

	string lineOfText;
	while (lineOfText != "eof"){
		//reads one line of text.
		lineOfText = fileManage.readOneLineOfText(inpFileDirectory);
		//adds to the map that holds <filename,lineoftext> and then creates the map for <word,1>
		mapFile->mapText(fileManage.getShortFileName(),lineOfText);
	}
}



