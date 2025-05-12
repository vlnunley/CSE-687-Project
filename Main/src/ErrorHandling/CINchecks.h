#pragma once
#include <string>

using std::string;

namespace CINchecks
{
	//All of the following do checks to see if the input type is correct, if not clears cin and reasks for input
	void inputDirectoryCheck(string& inputDir);
	void outputDirectoryCheck(string& outputDir);
	void tempDirectoryCheck(string& tempDir);
};