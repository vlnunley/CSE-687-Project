#include "CINchecks.h"
#include <iostream>
#include <filesystem>

using std::string;
using std::cin;
using std::cout;
using std::getline;
namespace fs = std::filesystem;

//checks to see if the input type is correct, if not clears cin and reasks for input
void CINchecks::inputDirectoryCheck(string& dir) {
	while (cin.fail() || !fs::is_directory(dir))
	{
		cout << "\nPlease enter a valid directory";
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "\tEnter directory containing input files: ";
		getline(cin, dir);
	}
}

void CINchecks::outputDirectoryCheck(string& dir) {
	while (cin.fail() || !fs::is_directory(dir))
	{
		cout << "\nPlease enter a valid directory";
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "\tEnter directory containing output files: ";
		getline(cin, dir);
	}
}

void CINchecks::tempDirectoryCheck(string& dir) {
	while (cin.fail() || !fs::is_directory(dir))
	{
		cout << "\nPlease enter a valid directory";
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "\tEnter directory containing temp files: ";
		getline(cin, dir);
	}
}