/*
CSE 687 Project
Contains the implementation of the FileManagement class
*/


#include "FileManagement.h"
#include <stdexcept>
#include <optional>

using std::ifstream;
using std::ofstream;
using std::getline;
using std::cout;
using std::runtime_error;
using std::string;
using std::vector;
using std::ios;
using std::optional;

namespace fs = std::filesystem;

FileManagement::FileManagement(string inputDir, string outputDir, string tempDir)
	: inputDirectory(inputDir), outputDirectory(outputDir), tempDirectory(tempDir), currentFileIndex(0)
{
	loadInputFiles();
	setCurrentInputFile(inputFiles[currentFileIndex]);
	openFile(currentFileIndex, true);
	removeOldFiles();
}

FileManagement::~FileManagement() {
	closeFile();
}

void FileManagement::loadInputFiles() {

	for (const auto& entry : fs::directory_iterator(inputDirectory)) {
		if (entry.is_regular_file()) {
			inputFiles.push_back(entry.path().string());
		}
	}

	if (inputFiles.empty()) {
		throw runtime_error("No files found in the input directory.");
	}
}

//Getters
vector<string> FileManagement::getInputFiles() {
	return inputFiles;
}

string FileManagement::getCurrentInputFile() {
	return currentFile;
}

//Setters
void FileManagement::setCurrentInputFile(string file) {
	currentFile = file;
}

//Actions
bool FileManagement::openFile(size_t i, bool isInput) {
	if (isInput) {
		if (i >= inputFiles.size()) {
			return false;
		}

		closeFile(); // Close previous if open

		currentFile = inputFiles[i];
		
	}
	else {
		currentFile = tempDirectory + "/tempFile.txt";
	}
	
	currentFileStream.open(currentFile);
	return currentFileStream.is_open();
}

void FileManagement::closeFile() {
	if (currentFileStream.is_open()) {
		currentFileStream.close();
	}
}

optional<string> FileManagement::readNextLine() {
	string line;
	if (getline(currentFileStream, line)) {
		return line;
	}
	return std::nullopt;  // Signals EOF
}

void FileManagement::writeToOutput(string filename, string content) {
	ofstream outFile(outputDirectory + "/" + filename, ios::app);
	if (!outFile.is_open()) {
		throw std::runtime_error("Failed to open the output file for writing.");
	}
	outFile << content;
	outFile.close();
}

void FileManagement::writeToTemp(string content) {
	ofstream tempFile(tempDirectory + "/tempFile.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content;
	tempFile.close();
}

void FileManagement::removeOldFiles() {
	cout << "\nRemoving old temporary and output files\n";

	for (const auto& entry : fs::directory_iterator(outputDirectory)) {
		fs::remove(entry.path().string());
	}
	for (const auto& entry : fs::directory_iterator(tempDirectory)) {
		fs::remove(entry.path().string());
	}
}
