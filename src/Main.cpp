
#include "FileManagement/FileManagement.h"
#include <iostream>
#include<string>
#include<vector>
#include <optional>
#include "Map/Map.h"
#include<algorithm>
#include "Reduce/Reduce.h"

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

int main()
{
	string inputDir;
	string outputDir;
	string tempDir;

	//grab input directory
	cout << "Enter directory containing input files: ";
	getline(cin, inputDir);
	
	//grab the output directory
	cout << "Enter directory for output files: ";
	getline(cin, outputDir);

	//grab the temporary directory
	cout << "Enter directory for temporary files: ";
	getline(cin, tempDir);

	try {
		FileManagement fileManager{inputDir, outputDir, tempDir};

		const auto& files = fileManager.getInputFiles();

		Map mapInst(fileManager);

		for (size_t i = 0; i < files.size(); ++i) {
			cout << "\n";
			cout << "Opening file: " << files[i] << endl;

			if (!fileManager.openFile(i, true)) {
				cerr << "Failed to open file: " << files[i] << endl;
				continue;
			}

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
			fileManager.closeFile(); // optional, destructor will handle it
		}

		map <string,vector<int>> tempFileLoaded;
		cout << "Opening temp file";
		size_t i = 0;
		if (!fileManager.openFile(i, false)) {
			cerr << "Failed to open temp";
		}
		else {
			optional<string> lineOpt = fileManager.readNextLine();
			while (lineOpt.has_value()) {
				string line = lineOpt.value();
				lineOpt = fileManager.readNextLine();
				cout << line + "\n";
				mapInst.sortWords(tempFileLoaded, line);
			}
		}
		
		Reduce reducer(fileManager);
		for (auto p : tempFileLoaded)
		{
			reducer.ReduceDown(p.first, p.second);
		}
		fileManager.writeToOutput("SUCCESS", "");
		
	}
	catch (const exception& e) {
		cerr << "Exception: " << e.what() << endl;
	}
	
}




