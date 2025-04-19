#pragma once
#include<string>
#include "../FileManagement/FileManagement.h"

class Reduce
{
public:
    Reduce(FileManagement& fileManager);
    ~Reduce();

    void reduce(const std::string& key, const std::vector<int>& values);

private:
    void export(const std::string& key, const int value);

    FileManagement& fileManagement;
};
