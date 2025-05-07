// ReduceLibrary.h - Contains declarations of reduce functions
#pragma once

#ifdef DLLTEST_EXPORTS
#define REDUCELIBRARY_API __declspec(dllexport)
#else
#define REDUCELIBRARY_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include "FileManagement.h"
using std::string;
using std::vector;

extern "C" REDUCELIBRARY_API void Reduce(FileManagement& fileManager);
extern "C" REDUCELIBRARY_API void ReduceDown(const string& key, const vector<int>& values);
extern "C" REDUCELIBRARY_API int testingDLL();