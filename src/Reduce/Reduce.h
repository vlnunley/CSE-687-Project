#pragma once
#include<string>
#include "../FileManagement/FileManagement.h"
#include<vector>

using std::string;
using std::vector;

class Reduce
{
private:
    void Export(const string& key, const int value);
    FileManagement& fileManagement;

public:
    Reduce(FileManagement& fileManager);
    void ReduceDown(const string& key, const vector<int>& values);
};