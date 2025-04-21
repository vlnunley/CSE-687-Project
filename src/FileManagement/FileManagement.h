/*
CSE 687 Project
Contains the definition of the FileManagement class
*/


#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <optional>

using std::string;
using std::vector;
using std::ifstream;
using std::optional;

class FileManagement
{
private:
	string inputDirectory;
	string outputDirectory;
	string tempDirectory;
	vector<string> inputFiles;
	string currentFile;
	size_t currentFileIndex;
	ifstream currentFileStream;

	void loadInputFiles();

public:
	FileManagement(string inputDir, string outputDir, string tempDir);
	~FileManagement();

	//Getters
	string getCurrentInputFile();
	vector<string> getInputFiles();

	//Setters
	void setCurrentInputFile(string file);

	//Actions
	bool openFile(size_t i, bool isInput);
	void closeFile();
	optional<string> readNextLine();
	void writeToTemp(string content);
	void writeToOutput(string filename, string content);
};

