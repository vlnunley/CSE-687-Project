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
#include <Thread>
#include<queue>
#include<mutex>
using std::runtime_error;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
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
using std::multimap;
using std::thread;
using CINchecks::inputDirectoryCheck;
using CINchecks::outputDirectoryCheck;
using CINchecks::tempDirectoryCheck;
using std::mutex;
void Mapthread(multimap<string,string>& text,FileManagement& fileManager, vector<mutex>& mtxVect);
//static mutex mtx;
//static mutex mtx1;
//static mutex mtx2;
//static mutex mtx3;
static mutex mtx4;
typedef void (*funcMap)(string, string);
typedef queue<pair<string, int>>(*funcMapExport)(FileManagement&);
typedef int (*funcReduce)(const string&, vector<int>&);


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
		HINSTANCE  hDLL2;


		//Map
		const auto& files = fileManager.getInputFiles();
		int count = 0;
		vector<multimap<string, string>> fullText(17);
		vector<mutex> mtxVect(8);
	
		
		for (size_t i = 0; i < files.size(); ++i) {
			cout << "\nOpening file: " << files[i] << endl;

			if (!fileManager.openFile(i, true)) {
				cerr << "Failed to open file: " << files[i] << endl;
				continue;
			}
			cout << "\tMapping content...\n";
			//Map map(fileManager);
			
			optional<string> lineOpt = fileManager.readNextLine();
		
			
			while (lineOpt.has_value()) {	
				string line = lineOpt.value();

				if(line!=""){
					pair <string, string> currentPair = pair<string, string>(fileManager.getCurrentInputFile(), line);
					fullText[i].insert(currentPair);
				}					
					lineOpt = fileManager.readNextLine();	
			}
		
		
			
			cout << "\tFinished - Closing file...\n";
			fileManager.closeFile(); // optional, destructor will handle it
		}

		vector<thread> threads;
		for (int i = 0; i < 17; i++) {
			threads.emplace_back(Mapthread, std::ref(fullText[i]), std::ref(fileManager),std::ref(mtxVect));
			cout << "Thread " << threads[i].get_id() << " has started"<< endl;
		}
		for (auto& t : threads) {
			t.join();
		}
		//FreeLibrary(hDLL);

		//Reduce
		map<string, vector<int>> tempFileLoaded;
		size_t i = 0;
		int wordSum = 0;
		string keysum;
		

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
			for (auto p : tempFileLoaded)
			{
				/*wordSum = reduceDown(p.first, p.second);*/
				keysum = "(" + p.first + "," + to_string(wordSum) + ")\n";
				fileManager.writeToOutput("Output.txt", keysum);
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


void Mapthread(multimap<string, string>& text, FileManagement& fileManager,vector<mutex>& mtxVect) {
	try{
	HINSTANCE hDLL= LoadLibraryEx(L"MapLibrary", NULL, NULL);
	if (!hDLL) {
		throw runtime_error("Failed to load DLL");
	}
	funcMap mapText;
	funcMapExport mapExport;
	mapText = (funcMap)GetProcAddress(hDLL, "mapText");
	mapExport = (funcMapExport)GetProcAddress(hDLL, "Export");
	if (!mapText || !mapExport) {
		FreeLibrary(hDLL);
		throw runtime_error("Failed to get function addresses");
	}
	int cnt = 0;
	srand(time(0) + std::hash<std::thread::id>{}(std::this_thread::get_id()));
	int writingFilesIndex = rand() % 8;
	//int writingFilesIndex = 0;
	for (const auto& line : text)
	{		
			mapText(line.first, line.second);		
			cnt++;
			if (cnt >= 500) {				
					queue<pair<string, int>> tempQueue = mapExport(fileManager);
						fileManager.WriteToMultipleTempFiles(tempQueue, fileManager, writingFilesIndex,mtxVect);
						cnt = 0;	
			}
	}
	// finish writing the remaining words.
	while (1) {
		queue<pair<string, int>> tempQueue = mapExport(fileManager);
		fileManager.WriteToMultipleTempFiles(tempQueue, fileManager, writingFilesIndex, mtxVect);
		break;
	}

	while (1) {
		if (mtx4.try_lock()) {
			cout << "Thread: " << std::this_thread::get_id() << " has finished." << endl;
			mtx4.unlock();
			break;
		}
		else {
			std::this_thread::yield();
		}
	}
	FreeLibrary(hDLL);
	}
	catch (std::exception Ex) {
		
	}
	

}




