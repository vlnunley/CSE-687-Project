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
static mutex mtx0;
static mutex mtx1;
static mutex mtx2;
static mutex mtx3;
static mutex mtx4;
static mutex mtx5;
static mutex mtx6;
static mutex mtx7;

FileManagement::FileManagement(string inputDir, string outputDir, string tempDir, bool removeOld)
	: inputDirectory(inputDir), outputDirectory(outputDir), tempDirectory(tempDir), currentFileIndex(0)
{
	loadInputFiles();
	setCurrentInputFile(inputFiles[currentFileIndex]);
	openFile(currentFileIndex, true);

	if (removeOld)
	{
		removeOldFiles();
	}
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
	closeFile();
	if (isInput) {
		if (i >= inputFiles.size()) {
			return false;
		}
		currentFile = inputFiles[i];
	}
	else {
		if (i == 0) {
			currentFile = tempDirectory + "/tempFile.txt";
		} else {
			currentFile = tempDirectory + "/tempFile" + std::to_string(i) + ".txt";
		}
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
void FileManagement::WriteToMultipleTempFiles(queue<pair<string, int>>& tempQueue, FileManagement& fileManager, int& writingFilesIndex) {
	switch (writingFilesIndex) {
	case 0: {
		while (1) {
			if (mtx0.try_lock()) {
				while (!tempQueue.empty()) {
				string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
				fileManager.writeToTemp(tempString);
				tempQueue.pop();
				}
				mtx0.unlock();
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
			if (mtx1.try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp1(tempString);
					tempQueue.pop();
				}
				mtx1.unlock();
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
			if (mtx2.try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp2(tempString);
					tempQueue.pop();
				}
				mtx2.unlock();
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
			if (mtx3.try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp3(tempString);
					tempQueue.pop();
				}
				mtx3.unlock();
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
			if (mtx4.try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp4(tempString);
					tempQueue.pop();
				}
				mtx4.unlock();
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
			if (mtx5.try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp5(tempString);
					tempQueue.pop();
				}
				mtx5.unlock();
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
			if (mtx6.try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp6(tempString);
					tempQueue.pop();
				}
				mtx6.unlock();
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
			if (mtx7.try_lock()) {
				while (!tempQueue.empty()) {
					string tempString = "(" + tempQueue.front().first + "," + std::to_string(tempQueue.front().second) + ")";
					fileManager.writeToTemp7(tempString);
					tempQueue.pop();
				}
				mtx7.unlock();
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

