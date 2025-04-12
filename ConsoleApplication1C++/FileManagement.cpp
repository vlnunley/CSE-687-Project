#include "FileManagement.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
using std::ifstream;
using std::getline;
using std::cout;
using std::exception;

   


FileManagement::FileManagement() {

}

void FileManagement::setInputFileDirectory(string value) {
	inputFileDirectory = value;
};
void FileManagement::setOutputFileDirectory(string value) {
	outputFileDirectory = value;
};
void FileManagement::setTempFileDirectory(string value) {
	inputFileDirectory = value;
};
void FileManagement::setShortFileName(string value) {
    shortFileName = value;
};

string FileManagement::getInputFileDirectory() {
    return inputFileDirectory;
};
string FileManagement::getOutputFileDirectory() {
    return outputFileDirectory;
};
string FileManagement::getTempFileDirectory() {
    return tempFileDirectory;
};

string FileManagement::getShortFileName() {
    return shortFileName;
};


string FileManagement::readOneLineOfText(string filePath) {
    
    string oneLineOfText;
  
    try {
        if (!file->is_open()) {
            file->open(filePath);
        }
        
        if (file->eof() == true) {
            return "eof";
        }
        if (file->is_open()) {
       
            std::getline(*file, oneLineOfText);
            return oneLineOfText;
        }

    }
    catch (exception &e) {
        cout << e.what();
    }
}

string FileManagement::parseShortFileName(string filePath) {
    int begin = filePath.find_last_of("\\");
    string shortPath = filePath.substr(begin+1, filePath.length());
    return shortPath;
}