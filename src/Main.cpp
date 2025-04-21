
#include "FileManagement/FileManagement.h"
#include <iostream>
#include<string>
#include<vector>
#include <optional>
#include "Map/Map.h"
#include<algorithm>
#include "Reduce/Reduce.h"
#include "ErrorHandling/CINchecks.h"

using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::getline;
using std::cerr;
using std::ofstream;
using std::optional;
using std::vector;
using std::map;
using std::find;
using std::exception;
using CINchecks::inputDirectoryCheck;
using CINchecks::outputDirectoryCheck;
using CINchecks::tempDirectoryCheck;

int main()
{
	string inputDir;
	string outputDir;
	string tempDir;

	//grab input directory
	cout << "Enter directory containing input files: ";
	getline(cin, inputDir);
	inputDirectoryCheck(inputDir);
	
	//grab the output directory
	cout << "\nEnter directory for output files: ";
	getline(cin, outputDir);
	outputDirectoryCheck(outputDir);

	//grab the temporary directory
	cout << "\nEnter directory for temporary files: ";
	getline(cin, tempDir);
	tempDirectoryCheck(tempDir);

	try {
		FileManagement fileManager{inputDir, outputDir, tempDir};

		const auto& files = fileManager.getInputFiles();

		Map mapInst(fileManager);

		for (size_t i = 0; i < files.size(); ++i) {
			cout << "\nOpening file: " << files[i] << endl;

			if (!fileManager.openFile(i, true)) {
				cerr << "Failed to open file: " << files[i] << endl;
				continue;
			}
			cout << "\tMapping content...\n";
			Map map(fileManager);
			optional<string> lineOpt = fileManager.readNextLine();
			while (lineOpt.has_value()) {
				string line = lineOpt.value();
				lineOpt = fileManager.readNextLine();
				map.mapText(fileManager.getCurrentInputFile(), line);
			}
			if (!map.getTempFileQueue().empty()) {
				map.Export();
			}
			cout << "\tFinished - Closing file...\n";
			fileManager.closeFile(); // optional, destructor will handle it
		}

		cout << "\nOpening temp file\n";
		map <string,vector<int>> tempFileLoaded;
		size_t i = 0;
		if (!fileManager.openFile(i, false)) {
			cerr << "Failed to open temp file";
		}
		else {
			cout << "\tSorting content...\n";
			optional<string> lineOpt = fileManager.readNextLine();
			while (lineOpt.has_value()) {
				string line = lineOpt.value();
				lineOpt = fileManager.readNextLine();
				mapInst.sortWords(tempFileLoaded, line);
			}
			cout << "\tReducing content...\n";
			Reduce reducer(fileManager);
			for (auto p : tempFileLoaded)
			{
				reducer.ReduceDown(p.first, p.second);
			}
			cout << "\nWriting results to file\n";
			fileManager.writeToOutput("SUCCESS", "");
			cout << "DONE\n";
		}
	}
	catch (const exception& e) {
		cerr << "Exception: " << e.what() << endl;
	}
	
}




