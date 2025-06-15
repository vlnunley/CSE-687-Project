#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <optional>
#include<algorithm>
#include "FileManagement.h"
#include "Utils.h"
#include<queue>
#include<mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <atomic>
#include <chrono>
#include <map>
#pragma comment(lib, "ws2_32.lib")

using std::string;
using std::multimap;
using std::runtime_error;

typedef void (*funcMap)(string, string);
typedef queue<pair<string, int>>(*funcMapExport)(FileManagement&);
typedef int (*funcReduce)(const string&, vector<int>&);
static mutex mtx4;

const int CONTROLLER_PORT = 6000;
const int HEARTBEAT_INTERVAL_MS = 2000; // 2 seconds


void sendHeartbeat(int stubId, const std::string& workerType, int workerId) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6000);  // controller port
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
        std::string msg = "Stub " + std::to_string(stubId) + " " + workerType + " " + std::to_string(workerId) + " heartbeat";
        send(sock, msg.c_str(), (int)msg.size(), 0);
    }
    closesocket(sock);
}

void mapThread(int stubId, int workerId, multimap<string, string> text) {
    FileManagement fileManager{ "./mapreduce/input", "./mapreduce/output", "./mapreduce/temp", false };

    std::cout << "Map worker " << workerId << " started processing in stub " << stubId << "\n";

    while (true) {
        string msg;

        try {
            HINSTANCE hDLL = LoadLibraryEx(L"MapLibrary", NULL, NULL);
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
            auto lastHeartbeat = std::chrono::steady_clock::now();

            for (const auto& line : text)
            {
                mapText(line.first, line.second);
                cnt++;
                if (cnt >= 500) {
                    queue<pair<string, int>> tempQueue = mapExport(fileManager);
                    fileManager.WriteToMultipleTempFiles(tempQueue, fileManager, writingFilesIndex);
                    cnt = 0;
                }

                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastHeartbeat).count() >= HEARTBEAT_INTERVAL_MS) {
                    sendHeartbeat(stubId, "Map", workerId);
                    lastHeartbeat = now;
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
            std::cout << Ex.what();
        }
    }

    // Send final heartbeat on completion
    sendHeartbeat(stubId, "Map", workerId);
    std::cout << "Map worker " << workerId << " finished processing in stub " << stubId << "\n";
}

void reduceThread(int stubId, int workerId, int index, multimap<string, int>& count) {
    std::cout << "Reduce worker " << workerId << " started processing in stub " << stubId << "\n";

    auto lastHeartbeat = std::chrono::steady_clock::now();


    while (true)
    {
        HINSTANCE hDLL;
        funcReduce reduceDown;
        Utils utils;
        FileManagement fileManager{ "./mapreduce/input", "./mapreduce/output", "./mapreduce/temp", false };

        try
        {
            const wchar_t* reduceLib = L"ReduceLibrary";
            hDLL = LoadLibraryEx(reduceLib, NULL, NULL);
            if (hDLL == NULL) {
                throw std::exception("Failed to load Reduce Library. Stopping program...");
            }

            reduceDown = (funcReduce)GetProcAddress(hDLL, "ReduceDown");
            if (reduceDown == NULL) {
                FreeLibrary(hDLL);
                throw std::exception("Reduce function failed to load. Stopping program...");
            }

            map<string, vector<int>> tempFileLoaded;
            int wordSum = 0;

            if (!fileManager.openFile(index, false)) {
                throw std::exception("Failed to open temp file");
            }

            std::optional<string> lineOpt = fileManager.readNextLine();
            while (lineOpt.has_value()) {
                string line = lineOpt.value();
                lineOpt = fileManager.readNextLine();
                utils.sortWords(tempFileLoaded, line);
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastHeartbeat).count() >= HEARTBEAT_INTERVAL_MS) {
                    sendHeartbeat(stubId, "Reduce", workerId);
                    lastHeartbeat = now;
                }

            }

            for (auto p : tempFileLoaded) {
                wordSum = reduceDown(p.first, p.second);
                count.insert(make_pair(p.first, wordSum));
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastHeartbeat).count() >= HEARTBEAT_INTERVAL_MS) {
                    sendHeartbeat(stubId, "Reduce", workerId);
                    lastHeartbeat = now;
                }

            }

            std::cout << "Thread: " << std::this_thread::get_id() << " has finished." << std::endl;
            FreeLibrary(hDLL);
        }
        catch (std::exception Ex) {
            std::cout << Ex.what();
        }
    }

    // Send final heartbeat on completion
    sendHeartbeat(stubId, "Reduce", workerId);
    std::cout << "Reduce worker " << workerId << " finished processing in stub " << stubId << "\n";

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: stub <stub_id>\n";
        return 1;
    }

    int stubId = std::stoi(argv[1]);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Listening socket to receive commands ("spawn")
    int port = 5000 + stubId;
    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    std::cout << "Socket created\n";

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    std::cout << "Bind succeeded\n";

    if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    std::cout << "Listen succeeded\n";

    std::cout << "[Stub " << stubId << "] Listening on port " << port << "\n";

    const int MAPS_PER_STUB = 5;
    const int REDUCE_NUM = 8;

    std::vector<std::thread> workers;

    while (true) {
        std::cout << "[Stub " << stubId << "] Waiting for a client connection...\n";

        SOCKET clientSock = accept(serverSock, nullptr, nullptr);
        FileManagement fileManager{ "./mapreduce/input", "./mapreduce/output", "./mapreduce/temp", false };

        std::cout << "[Stub " << stubId << "] accept() returned.\n";
        if (clientSock == INVALID_SOCKET) {
            std::cerr << "[Stub " << stubId << "] Accept failed with error: " << WSAGetLastError() << "\n";
            continue;
        }
        std::cout << "[Stub " << stubId << "] Got a connection!\n";

        char buffer[1024] = { 0 };
        int received = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            std::string msg(buffer, received);
            std::cout << "Stub " << stubId << " received: " << msg << "\n";

            if (msg == "spawn map") {
                
                const auto& allFiles = fileManager.getInputFiles();


                int startIndex = stubId * MAPS_PER_STUB;
                int endIndex = startIndex + MAPS_PER_STUB;

                std::vector<std::string> myFiles(allFiles.begin() + startIndex, allFiles.begin() + endIndex);
                // Spawn threads for map workers
                for (int i = 0; i < MAPS_PER_STUB; ++i) {
                    std::cout << "\nOpening file: " << myFiles[i] << std::endl;

                    if (!fileManager.openFile(startIndex + i, true)) {
                        std::cerr << "Failed to open file: " << myFiles[i] << std::endl;
                    }

                    optional<string> lineOpt = fileManager.readNextLine();
                    multimap<string, string> fullText;

                    std::cout << "\nReading file...\n";

                    while (lineOpt.has_value()) {
                        string line = lineOpt.value();

                        if (line != "") {
                            fullText.insert(pair<string, string>(fileManager.getCurrentInputFile(), line));
                        }
                        lineOpt = fileManager.readNextLine();
                    }
                    std::cout << "\tFinished - Closing file...\n";
                    fileManager.closeFile(); // optional, destructor will handle it
                    workers.emplace_back(mapThread, stubId, i, fullText);
                }
            }
            if (msg == "spawn reduce")
            {
                vector<multimap<string, int>> reduce_text(REDUCE_NUM);
                for (int i = 0; i < REDUCE_NUM; ++i) {
                    // Spawn thread for reduce worker
                    workers.emplace_back(reduceThread, stubId, 0, i, std::ref(reduce_text[i]));
                }

                multimap<string, int> reduce_results;
                for (int i = 0; i < REDUCE_NUM; i++) {
        	        for (const auto& p : reduce_text[i]) {
        		        int wordSum = 0;
        		        auto it = reduce_results.find(p.first);
        		        if (it != reduce_results.end()) {
        			        wordSum = it->second + p.second;
        			        reduce_results.erase(it);
        			        reduce_results.insert(std::make_pair(p.first, wordSum));
        		        }
        		        else {
        			        wordSum = p.second;
        			        reduce_results.insert(std::make_pair(p.first, wordSum));
        		        }
        	        }
                }

                std::cout << "\nWriting results to file\n";
                for (auto& p : reduce_results) {
                	string keysum;
                	keysum = "(" + p.first + "," + std::to_string(p.second) + ")\n";
                	fileManager.writeToOutput("Output.txt", keysum);
                }
                fileManager.writeToOutput("SUCCESS", "");
                
            }
            for (auto& t : workers) {
                if (t.joinable()) t.join();
            }
        }
        closesocket(clientSock);
    }


    WSACleanup();
    return 0;
}