#pragma once
#include<string>
#include <fstream>
using std::string;
using std::ifstream;
class FileManagement
{

	string inputFileDirectory;
	string shortFileName;
	string outputFileDirectory;
	string tempFileDirectory;
	
	 ifstream* file = new ifstream();
public:

	FileManagement();

	void setInputFileDirectory(string value);
	void setOutputFileDirectory(string value);
	void setTempFileDirectory(string value);
	void setShortFileName(string value);

	string getInputFileDirectory();
	string getOutputFileDirectory();
	string getTempFileDirectory();
	string getShortFileName();

	string readOneLineOfText(string filePath);
	string parseShortFileName(string filePath);
};

