#pragma once
#include<string>

class Reduce
{
public:
    void reduce(const std::string& key, const std::vector<int>& values);

private:
    void export(const std::string& key, const int value);
};
