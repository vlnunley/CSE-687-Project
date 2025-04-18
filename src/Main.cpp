
#include "FileManagement/FileManagement.h"
#include <iostream>
#include<string>
#include<vector>
#include <optional>

using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::getline;
using std::cerr;
using std::ofstream;
using std::optional;


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
		FileManagement fileManager{ inputDir, outputDir, tempDir };
		const auto& files = fileManager.getInputFiles();

		for (size_t i = 0; i < files.size(); ++i) {
			cout << "\n";
			cout << "Opening file: " << files[i] << endl;

			if (!fileManager.openFile(i)) {
				cerr << "Failed to open file: " << files[i] << endl;
				continue;
			}

			optional<string> lineOpt = fileManager.readNextLine();
			while (lineOpt.has_value()) {
				string line = lineOpt.value();
				cout << "  Line: " << line << endl;
				lineOpt = fileManager.readNextLine();
			}

			fileManager.closeFile(); // optional, destructor will handle it
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << endl;
	}
	
}



