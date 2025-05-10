#include "CINchecks.h"
#include <iostream>
#include <filesystem>

using std::string;
using std::cin;
using std::cout;
using std::getline;
namespace fs = std::filesystem;

//checks to see if the input type is correct, if not clears cin and reasks for input
void CINchecks::dllDirectoryCheck(string& dir) {
	while (cin.fail() || !fs::is_directory(dir) || fs::is_empty(dir))
	{
		if (dir.empty())
		{
			cout << "\tUsing deafult DLL directory.\n";
			dir = ".\\mapreduce\\dlls";
			break;
		}
		if(!fs::is_directory(dir)) cout << "\nDirectory not found. Please try again";

		if (fs::is_empty(dir)) cout << "\nDirectory is empty. Please try again.";

		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "\tEnter directory containing DLLs: ";
		getline(cin, dir);
	}
}

void CINchecks::inputDirectoryCheck(string& dir) {
	while (cin.fail() || !fs::is_directory(dir) || fs::is_empty(dir))
	{
		if (!fs::is_directory(dir)) cout << "\nDirectory not found. Please try again";

		if (fs::is_empty(dir)) cout << "\nDirectory is empty. Please try again.";

		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "\tEnter directory containing input files: ";
		getline(cin, dir);
	}
}

void CINchecks::outputDirectoryCheck(string& dir) {
	while (cin.fail() || !fs::is_directory(dir))
	{
		if (dir.empty())
		{
			cout << "\tUsing deafult output directory.\n";
			dir = "./mapreduce/output";
			break;
		}

		if (!fs::is_directory(dir)) cout << "\nDirectory not found. Please try again";

		if (fs::is_empty(dir)) cout << "\nDirectory is empty. Please try again.";

		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "\tEnter directory containing output files: ";
		getline(cin, dir);
	}
}

void CINchecks::tempDirectoryCheck(string& dir) {
	while (cin.fail() || !fs::is_directory(dir))
	{
		if (dir.empty())
		{
			cout << "\tUsing deafult temp directory.\n";
			dir = "./mapreduce/temp";
			break;
		}

		if (!fs::is_directory(dir)) cout << "\nDirectory not found. Please try again";

		if (fs::is_empty(dir)) cout << "\nDirectory is empty. Please try again.";

		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "\tEnter directory containing temp files: ";
		getline(cin, dir);
	}
}