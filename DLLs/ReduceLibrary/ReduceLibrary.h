// ReduceLibrary.h - Contains declarations of reduce functions
#pragma once

#ifdef REDUCELIBRARY_EXPORTS
#define REDUCELIBRARY_API __declspec(dllexport)
#else
#define REDUCELIBRARY_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
using std::string;
using std::vector;

extern "C" REDUCELIBRARY_API int ReduceDown(const string& key, vector<int>& values);