#include <iostream>

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
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib") 
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

void Mapthread(multimap<string,string> text);
void ReduceThread(int index, multimap<string, int>& count);
void StubProcess(int port);
//static mutex mtx;
//static mutex mtx1;
//static mutex mtx2;
//static mutex mtx3;
static mutex mtx4;
typedef void (*funcMap)(string, string);
typedef queue<pair<string, int>>(*funcMapExport)(FileManagement&);
typedef int (*funcReduce)(const string&, vector<int>&);

const int R = 8; // Number of reduce threads
vector<multimap<string, string>> fullText(17);
static mutex mapThreadCountmtx;
int mapThreadCount = 0;
vector<thread> mapThreads;
static mutex mapThreadsmtx;

static mutex reduceThreadCountmtx;
int reduceThreadCount = 0;
vector<thread> reduceThreads;
static mutex reduceThreadsmtx;
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
		FileManagement fileManager{ inputDir, outputDir, tempDir, false };

		//Map
		const auto& files = fileManager.getInputFiles();
		int count = 0;
		
		//vector<mutex> mtxVect(8);
	
		
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

		//initialization that needs to be done before using sockets in windows.
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cerr << "WSAStartup failed!" << std::endl;
			return -1;
		}
	
		//stub process listening for messages.
		std::thread StubProc1(StubProcess,8080);
		StubProc1.detach();
		std::thread StubProc2(StubProcess, 8081);
		StubProc2.detach();
		std::thread StubProc3(StubProcess, 8082);
		StubProc3.detach();

///////////////////////////creates mapThreadclients///////////////////////////
// 
		int controller = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in stubProc1Addr{};
		stubProc1Addr.sin_family = AF_INET;
		stubProc1Addr.sin_port = htons(8080);
		inet_pton(AF_INET, "192.168.56.1", &stubProc1Addr.sin_addr);

		if (connect(controller, (struct sockaddr*)&stubProc1Addr, sizeof(stubProc1Addr)) < 0) {
			std::cerr << "Failed to connect to server1!\n";
			return -1;
		}

		std::cout << "Connected to stub1!\n";

		//have stub1 create 5 mappers.
		std::string message = "CreateMapThread";
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		closesocket(controller);

		controller = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in stubProc2Addr{};
		stubProc2Addr.sin_family = AF_INET;
		stubProc2Addr.sin_port = htons(8081);
		inet_pton(AF_INET, "192.168.56.1", &stubProc2Addr.sin_addr);

		if (connect(controller, (struct sockaddr*)&stubProc2Addr, sizeof(stubProc2Addr)) < 0) {
			std::cerr << "Failed to connect to server2!\n";
			return -1;
		}
		std::cout << "Connected to stub2!\n";
	
		
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		
		closesocket(controller);
		controller = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in stubProc3Addr{};
		stubProc3Addr.sin_family = AF_INET;
		stubProc3Addr.sin_port = htons(8082);
		inet_pton(AF_INET, "192.168.56.1", &stubProc3Addr.sin_addr);

		if (connect(controller, (struct sockaddr*)&stubProc3Addr, sizeof(stubProc3Addr)) < 0) {
			std::cerr << "Failed to connect to server3!\n";
			return -1;
		}
		std::cout << "Connected to stub3!\n";

		

		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		Sleep(50);
		send(controller, message.c_str(), message.length(), 0);
		closesocket(controller);

///////////////////////////creates mapThreadclients///////////////////////////

		// Reduce
		cin.get();

///////////////////////////creates reduceThreadclients///////////////////////////

		std::string message = "CreateReduceThread";

		controller = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(controller, (struct sockaddr*)&stubProc1Addr, sizeof(stubProc1Addr)) < 0) {
			std::cerr << "Failed to connect to server1!\n";
			return -1;
		}
		std::cout << "Connected to stub1!\n";
		////have stub1 create 3 reducers.
		for (int i = 0; i < 3; i++) {
			send(controller, message.c_str(), message.length(), 0);
			Sleep(50);
		}
		closesocket(controller);

		controller = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(controller, (struct sockaddr*)&stubProc2Addr, sizeof(stubProc2Addr)) < 0) {
			std::cerr << "Failed to connect to server2!\n";
			return -1;
		}
		std::cout << "Connected to stub2!\n";
		for (int i = 0; i < 3; i++) {
			send(controller, message.c_str(), message.length(), 0);
			Sleep(50);
		}
		closesocket(controller);
		controller = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(controller, (struct sockaddr*)&stubProc3Addr, sizeof(stubProc3Addr)) < 0) {
			std::cerr << "Failed to connect to server3!\n";
			return -1;
		}
		std::cout << "Connected to stub3!\n";
		for (int i = 0; i < 2; i++) {
			send(controller, message.c_str(), message.length(), 0);
			Sleep(50);
		}

///////////////////////////creates reduceThreadclients///////////////////////////

	/*	vector<thread> threads;

		for (int i = 0; i < 17; i++) {
			threads.emplace_back(Mapthread, std::ref(fullText[i]));
			cout << "Thread " << threads[i].get_id() << " has started"<< endl;
		}
		for (auto& t : threads) {
			t.join();
		}*/

		//FreeLibrary(hDLL);

		//vector<multimap<string, int>> reduce_result(R);
		//vector<thread> reduce_threads;
		//for (int i = 0; i < R; i++) {
		//	reduce_threads.emplace_back(ReduceThread, i, std::ref(reduce_result[i]));
		//	cout << "Reduce thread " << reduce_threads[i].get_id() << " has started"<< endl;
		//}
		//for (auto& t : reduce_threads) {
		//	t.join();
		//}
		//
		// Reduction of results
		/*cout << "\nMerging results from all reduce threads...\n";
		multimap<string, int> reduce_results;
		for (int i = 0; i < R; i++) {
			for (const auto& p : reduce_result[i]) {
				int wordSum = 0;
				auto it = reduce_results.find(p.first);
				if (it != reduce_results.end()) {
					wordSum = it->second + p.second;
					reduce_results.erase(it);
					reduce_results.insert(make_pair(p.first, wordSum));
				} else {
					wordSum = p.second;
					reduce_results.insert(make_pair(p.first, wordSum));
				}
			}
		}

		cout << "\nWriting results to file\ n";
		for (auto& p : reduce_results) {
			string keysum;
			keysum = "(" + p.first + "," + to_string(p.second) + ")\n";
			fileManager.writeToOutput("Output.txt", keysum);
		}
		fileManager.writeToOutput("SUCCESS", "");
		cout << "DONE\n";*/
	}
	catch (const exception& e) {
		cerr << "Exception: " << e.what() << endl;
	}
}

void sendHeartbeat() {
	while (true) {
		// Simulate sending a heartbeat message
		Sleep(5000); // Sleep for 5 second

		int controller = socket(AF_INET, SOCK_STREAM, 0);
		string heartbeatMessage = "HEARTBEAT";
		send(controller, heartbeatMessage.c_str(), heartbeatMessage.length(), 0);
	}
}

void Mapthread(multimap<string, string> text) {
	std::thread heartbeatThread(sendHeartbeat);

	while (true) {
		string msg;
		if (msg != 'START_MAP')
		  continue;

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

			FileManagement fileManager{"C:\\Users\\Nate\\Documents\\shakespeare\\shakespeare","C:\\Users\\Nate\\Documents\\shakespeare\\output","C:\\Users\\Nate\\Documents\\shakespeare\\Temp",false};

			srand(time(0) + std::hash<std::thread::id>{}(std::this_thread::get_id()));
			int writingFilesIndex = rand() % 8;
			//int writingFilesIndex = 0;

			for (const auto& line : text)
			{
				mapText(line.first, line.second);
				cnt++;
				if (cnt >= 500) {
					queue<pair<string, int>> tempQueue = mapExport(fileManager);
					fileManager.WriteToMultipleTempFiles(tempQueue, fileManager, writingFilesIndex);
					cnt = 0;
				}
			}
			// finish writing the remaining words.
			//while (1) {
				queue<pair<string, int>> tempQueue = mapExport(fileManager);
				fileManager.WriteToMultipleTempFiles(tempQueue, fileManager, writingFilesIndex);
			/*	break;
			}*/

			while (1) {
				if (mtx4.try_lock()) {
					cout << "Thread: " << std::this_thread::get_id() << " has finished." << endl;
					mtx4.unlock();
					break;
				}
				else {
					Sleep(10);
				}
			}
			FreeLibrary(hDLL);
		}
		catch (std::exception Ex) {
			cout << Ex.what();
		}
	}
}

void ReduceThread(int index, multimap<string, int>& count) {
	std::thread heartbeatThread(sendHeartbeat);

	while (true) {
		string msg;
		if (msg != 'START_MAP')
		  continue;

		HINSTANCE hDLL;
		funcReduce reduceDown;
		Utils utils;
		FileManagement fileManager{ "C:\\Users\\Nate\\Documents\\shakespeare\\shakespeare","C:\\Users\\Nate\\Documents\\shakespeare\\output","C:\\Users\\Nate\\Documents\\shakespeare\\Temp",false };

		try {
			const wchar_t* reduceLib = L"ReduceLibrary";
			hDLL = LoadLibraryEx(reduceLib, NULL, NULL);
			if (hDLL == NULL) {
				throw exception("Failed to load Reduce Library. Stopping program...");
			}

			reduceDown = (funcReduce)GetProcAddress(hDLL, "ReduceDown");
			if (reduceDown == NULL) {
				FreeLibrary(hDLL);
				throw exception("Reduce function failed to load. Stopping program...");
			}

			map<string, vector<int>> tempFileLoaded;
			int wordSum = 0;

			if (!fileManager.openFile(index, false)) {
				throw exception("Failed to open temp file");
			}

			optional<string> lineOpt = fileManager.readNextLine();
			while (lineOpt.has_value()) {
				string line = lineOpt.value();
				lineOpt = fileManager.readNextLine();
				utils.sortWords(tempFileLoaded, line);
			}

			for (auto p : tempFileLoaded) {
				wordSum = reduceDown(p.first, p.second);
				count.insert(make_pair(p.first, wordSum));
			}

			cout << "Thread: " << std::this_thread::get_id() << " has finished." << endl;
			FreeLibrary(hDLL);
		}
		catch (std::exception Ex) {
			cout << Ex.what();
		}
	}
}


void StubProcess(int port) {
	try {
	//stub process listening for messages.
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(serverSocket, 5);
	unsigned long mode = 1;
	//ioctlsocket(serverSocket, FIONBIO, &mode);
	int clientCount = 0;
	int clientSocket;
	
	
	// Keep the server running
	 

		sockaddr_in clientAddr{};
		int clientLen = sizeof(clientAddr);
	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
	vector<multimap<string, int>> reduce_result(R);
	while (true) {  
		//if (clientCount < 3) {
		//	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
		//}
		

		if (clientSocket < 0) {
			std::cerr << "Failed to accept connection\n";
			continue;
		}
		++clientCount;


		char buffer[1024] = { 0 };
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived == 0) {
			clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
		}

		if (bytesReceived > 0) {
			buffer[bytesReceived] = '\0';  // Null-terminate the message
			//std::cout << "Received message: " << buffer << std::endl;
			string msg(buffer);
			msg.erase(std::remove_if(msg.begin(), msg.end(), ::isspace), msg.end());
			if (msg == "CreateMapThread") {
				while (1) {
					if (mapThreadsmtx.try_lock()) {
						if (mapThreadCount < 17) {				
							mapThreads.emplace_back(Mapthread, fullText[mapThreadCount]);
							cout << "MapThread " << mapThreads[mapThreadCount].get_id() << " has started" << endl;
							mapThreadsmtx.unlock();
							while (1) {
								if (mapThreadCountmtx.try_lock()) {
									mapThreadCount++;
									mapThreadCountmtx.unlock();
									break;
								}
								else {
									Sleep(5);
								}
							}
						}
						break;
					}else {
						Sleep(5);
					}
				}
			}else if (msg == "CreateReduceThread") {
				while (1) {
					if (reduceThreadsmtx.try_lock()) {
						if (reduceThreadCount < 8) {
							reduceThreads.emplace_back(ReduceThread,reduceThreadCount, std::ref(reduce_result[reduceThreadCount]));
							cout << "reduceThread " << reduceThreads[reduceThreadCount].get_id() << " has started" << endl;
							reduceThreadsmtx.unlock();
							while (1) {
								if (reduceThreadCountmtx.try_lock()) {
									reduceThreadCount++;
									reduceThreadCountmtx.unlock();
									break;
								}
								else {
									Sleep(5);
								}
							}
						}
						break;
					}
					else {
						Sleep(5);
					}
				}
			}
		}
	}

	//for (auto& t : mapThreads) {
	//	t.join();
	//}
	for (auto& t : reduceThreads) {
		t.join();
	}
	cout << "\nMerging results from all reduce threads...\n";
	multimap<string, int> reduce_results;
	for (int i = 0; i < R; i++) {
		for (const auto& p : reduce_result[i]) {
			int wordSum = 0;
			auto it = reduce_results.find(p.first);
			if (it != reduce_results.end()) {
				wordSum = it->second + p.second;
				reduce_results.erase(it);
				reduce_results.insert(make_pair(p.first, wordSum));
			}
			else {
				wordSum = p.second;
				reduce_results.insert(make_pair(p.first, wordSum));
			}
		}
	}

	cout << "\nWriting results to file\ n";

	FileManagement fileManager{ "C:\\Users\\Nate\\Documents\\shakespeare\\shakespeare","C:\\Users\\Nate\\Documents\\shakespeare\\output","C:\\Users\\Nate\\Documents\\shakespeare\\Temp",false };
	for (auto& p : reduce_results) {
		string keysum;
		keysum = "(" + p.first + "," + to_string(p.second) + ")\n";
		fileManager.writeToOutput("Output.txt", keysum);
	}
	fileManager.writeToOutput("SUCCESS", "");
	cout << "DONE\n";


	}
	catch (exception Ex) {
		//cout << Ex.what();
	}

}



 
