#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include<string>
#include<vector>
#include <optional>
#include<algorithm>
#include "ErrorHandling/CINchecks.h"
#include "FileManagement/FileManagement.h"
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
using CINchecks::ipAddress;
using std::mutex;

void sendMessage(const std::string& message, int port);
void listenForHeartbeats();

const int NUM_STUBS = 3;
const int MAPS_PER_STUB = 5;
const int REDUCES_PER_STUB = 2;

const int SPAWN_PORT_BASE = 5000;
const int START_SIGNAL_PORT = 4000;
const int CONTROLLER_PORT = 6000;

int mapDone = 0;

int main()
{
	string inputDir;
	string outputDir;
	string tempDir;
	string dllDir;
	string ip;
	
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
		FileManagement fileManager{ inputDir, outputDir, tempDir, true };

		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		// 1) Send spawn to all stubs
		for (int stubId = 0; stubId < NUM_STUBS; ++stubId) {
			int port = SPAWN_PORT_BASE + stubId;
			std::cout << "Sending 'spawn map' to stub " << stubId << " on port " << port << "\n";
			sendMessage("spawn map", port);
		}

		listenForHeartbeats();

		// Wait a bit for stubs to spawn threads
		std::this_thread::sleep_for(std::chrono::seconds(3));
		
		while (mapDone < 14)
		{}

		int port = SPAWN_PORT_BASE + 3;
		std::cout << "Sending 'spawn reduce' to stub " << 3 << " on port " << port << "\n";
		sendMessage("spawn reduce", port);


		WSACleanup();
		cout << "DONE\n";
		return 0;



	}
	catch (const exception& e) {
		cerr << "Exception: " << e.what() << endl;
	}
}

void sendMessage(const std::string& message, int port) {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cerr << "Socket creation failed\n";
		return;
	}

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
		send(sock, message.c_str(), (int)message.size(), 0);
	}
	else {
		std::cerr << "Failed to connect to port " << port << " to send: " << message << "\n";
	}

	closesocket(sock);
}


void listenForHeartbeats() {
	// Initialize Winsock
	WSADATA wsaData;
	int wsaInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaInit != 0) {
		std::cerr << "WSAStartup failed: " << wsaInit << "\n";
		return;
	}

	// Create listening socket
	SOCKET serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSock == INVALID_SOCKET) {
		std::cerr << "Socket creation failed\n";
		WSACleanup();
		return;
	}

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6000);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed: " << WSAGetLastError() << "\n";
		closesocket(serverSock);
		WSACleanup();
		return;
	}

	if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
		closesocket(serverSock);
		WSACleanup();
		return;
	}

	std::cout << "[Controller] Listening for heartbeats on port 6000...\n";

	while (true) {
		// Use select to wait up to 30 seconds for a connection
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(serverSock, &readfds);

		timeval timeout;
		timeout.tv_sec = 30;
		timeout.tv_usec = 0;

		int activity = select(0, &readfds, NULL, NULL, &timeout);

		if (activity == SOCKET_ERROR) {
			std::cerr << "Select failed: " << WSAGetLastError() << "\n";
			break;
		}

		if (activity == 0) {
			std::cout << "[Controller] No heartbeat received in 30 seconds. Exiting.\n";
			break;
		}

		// Accept incoming connection
		SOCKET clientSock = accept(serverSock, nullptr, nullptr);
		if (clientSock == INVALID_SOCKET) {
			std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
			continue;
		}

		// Receive heartbeat message
		char buffer[1024] = { 0 };
		int received = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
		if (received > 0) {
			std::string msg(buffer, received);
			std::cout << "[Heartbeat] " << msg << "\n";
			if (msg == "Map Done")
			{
				mapDone++;
			}
		}
		else {
			std::cerr << "[Controller] Received empty or failed message\n";
		}

		closesocket(clientSock);
	}

	closesocket(serverSock);
	WSACleanup();
}


 
