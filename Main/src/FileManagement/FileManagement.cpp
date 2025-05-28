/*
CSE 687 Project
Contains the implementation of the FileManagement class
*/

#include "FileManagement.h"
#include <stdexcept>
#include <optional>
#include <mutex>
#include <windows.h>
using std::ifstream;
using std::ofstream;
using std::getline;
using std::cout;
using std::runtime_error;
using std::string;
using std::vector;
using std::ios;
using std::optional;
using std::endl;
using std::mutex;

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
	outFile << content << endl;
	outFile.close();
}

void FileManagement::writeToTemp(string content) {
	
		ofstream tempFile(tempDirectory + "/tempFile.txt", ios::app);
		if (!tempFile.is_open()) {
			throw std::runtime_error("Failed to open the temporary file for writing.");
		}
		tempFile << content << endl;
		tempFile.close();
}
void FileManagement::writeToTemp1(string content) {

	ofstream tempFile(tempDirectory + "/tempFile1.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content << endl;
	tempFile.close();
}
void FileManagement::writeToTemp2(string content) {

	ofstream tempFile(tempDirectory + "/tempFile2.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content << endl;
	tempFile.close();
}
void FileManagement::writeToTemp3(string content) {

	ofstream tempFile(tempDirectory + "/tempFile3.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content << endl;
	tempFile.close();
}
void FileManagement::writeToTemp4(string content) {

	ofstream tempFile(tempDirectory + "/tempFile4.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content << endl;
	tempFile.close();
}
void FileManagement::writeToTemp5(string content) {

	ofstream tempFile(tempDirectory + "/tempFile5.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content << endl;
	tempFile.close();
}
void FileManagement::writeToTemp6(string content) {

	ofstream tempFile(tempDirectory + "/tempFile6.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content << endl;
	tempFile.close();
}
void FileManagement::writeToTemp7(string content) {

	ofstream tempFile(tempDirectory + "/tempFile7.txt", ios::app);
	if (!tempFile.is_open()) {
		throw std::runtime_error("Failed to open the temporary file for writing.");
	}
	tempFile << content << endl;
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
void FileManagement::WriteToMultipleTempFiles(queue<pair<string, int>>& tempQueue, FileManagement& fileManager, int& writingFilesIndex, vector<mutex>& mtxVect) {
	switch (writingFilesIndex) {
	case 0: {
		while (1) {
			if (mtxVect[0].try_lock()) {
				while (!tempQueue.empty()) {
				string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
				fileManager.writeToTemp(tempString);
				tempQueue.pop();
				}				
				mtxVect[0].unlock();
				writingFilesIndex = 1;			
				break;
			}
			else {
				std::this_thread::yield();
			}



		}
		break;
	}
	case 1: {
		while (1) {
			if (mtxVect[1].try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp1(tempString);
					tempQueue.pop();
				}
				mtxVect[1].unlock();
				writingFilesIndex = 2;
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		break;
	}
	case 2: {
		while (1) {
			if (mtxVect[2].try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp2(tempString);
					tempQueue.pop();
				}
				mtxVect[2].unlock();
				writingFilesIndex = 3;
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		break;
	}
	case 3: {
		while (1) {
			if (mtxVect[3].try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp3(tempString);
					tempQueue.pop();
				}
				mtxVect[3].unlock();
				writingFilesIndex = 4;
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		break;
	}
	case 4: {
		while (1) {
			if (mtxVect[4].try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp4(tempString);
					tempQueue.pop();
				}
				mtxVect[4].unlock();
				writingFilesIndex = 5;
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		break;
	}
	case 5: {
		while (1) {
			if (mtxVect[5].try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp5(tempString);
					tempQueue.pop();
				}
				mtxVect[5].unlock();
				writingFilesIndex = 6;
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		break;
	}
	case 6: {
		while (1) {
			if (mtxVect[6].try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp6(tempString);
					tempQueue.pop();
				}
				mtxVect[6].unlock();
				writingFilesIndex = 7;
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		break;
	}
	case 7: {
		while (1) {
			if (mtxVect[7].try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp7(tempString);
					tempQueue.pop();
				}
				mtxVect[7].unlock();
				writingFilesIndex = 0;
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		break;
	}
	}

}

