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
#include <mutex>
#include <queue>
#include <thread>
using std::string;
using std::vector;
using std::ifstream;
using std::optional;
using std::mutex;
using std::queue;
using std::pair;
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
	FileManagement(string inputDir, string outputDir, string tempDir, bool removeOld);
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
	void writeToTemp1(string content);
	void writeToTemp2(string content);
	void writeToTemp3(string content);
	void writeToTemp4(string content);
	void writeToTemp5(string content);
	void writeToTemp6(string content);
	void writeToTemp7(string content);
	void WriteToMultipleTempFiles(queue<pair<string, int>>& tempQueue, FileManagement& fileManager, int& writingFilesIndex);
	void writeToOutput(string filename, string content);
	void removeOldFiles();
};

