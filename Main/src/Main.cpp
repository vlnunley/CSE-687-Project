#include <iostream>
#include <windows.h>
#include<string>
#include<vector>
#include <optional>
#include<algorithm>
#include "Reduce/Reduce.h"
#include "ErrorHandling/CINchecks.h"
#include "FileManagement/FileManagement.h"
#include "Map/Map.h"
#include "Utils/Utils.h"

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
using std::queue;
using std::pair;
using std::map;
using CINchecks::inputDirectoryCheck;
using CINchecks::outputDirectoryCheck;
using CINchecks::tempDirectoryCheck;

typedef void (*funcMap)(string, string);
typedef void (*funcMapExport)(FileManagement&);

int main()
{
	Utils utils;
	string inputDir;
	string outputDir;
	string tempDir;
	string dllDir;

	//grab dll directory
	//cout << "Enter directory containing DLLs (Default: ./mapreduce/dlls): ";
	//getline(cin, dllDir);
	//dllDirectoryCheck(dllDir);

	//grab input directory
	cout << "Enter directory containing input files: ";
	getline(cin, inputDir);
	inputDirectoryCheck(inputDir);
	
	//grab the output directory
	cout << "\nEnter directory for output files (Default: ./mapreduce/output): ";
	getline(cin, outputDir);
	outputDirectoryCheck(outputDir);

	//grab the temporary directory
	cout << "\nEnter directory for temporary files (Default: ./mapreduce/temp): ";
	getline(cin, tempDir);
	tempDirectoryCheck(tempDir);

	try {
		FileManagement fileManager{ inputDir, outputDir, tempDir };

		//Load Map DLL
		HINSTANCE hDLL;
		funcMap mapText;
		funcMapExport mapExport;

		const wchar_t* mapLib = L"MapLibrary";
		hDLL = LoadLibraryEx(mapLib, NULL, NULL);

		if (hDLL == NULL) {
			throw exception("Failed to load Map Library. Stopping program...");
		}

		mapText = (funcMap)GetProcAddress(hDLL, "mapText");
		mapExport = (funcMapExport)GetProcAddress(hDLL, "Export");

		//Load Reduce DLL

		//Map
		const auto& files = fileManager.getInputFiles();
		int count = 0;

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
				if (count >= 100) {
					mapExport(fileManager);
					count = 0;
				}
				string line = lineOpt.value();
				mapText(fileManager.getCurrentInputFile(), line);
				lineOpt = fileManager.readNextLine();
				count++;
			}

			mapExport(fileManager);

			cout << "\tFinished - Closing file...\n";
			fileManager.closeFile(); // optional, destructor will handle it
		}

		FreeLibrary(hDLL);

		//Reduce

	}
	catch (const exception& e) {
		cerr << "Exception: " << e.what() << endl;
	}

		/*cout << "\nOpening temp file\n";
		map<string,vector<int>> tempFileLoaded;
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
				utils.sortWords(tempFileLoaded, line);
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
		}*/
	
}




