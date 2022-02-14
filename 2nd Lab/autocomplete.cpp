#include <string>
#include <iostream>
#include <vector>
#include <dirent.h>

using namespace std;

int main()
{
	vector <string> fileList;
	struct dirent * file;
	DIR * dir = opendir(".");
	while ((file = readdir(dir)) != NULL)
	{
		fileList.push_back(string(file->d_name));
	}
	for (auto x : fileList)
	{
		cout << x << endl;
	}
	string queryString;
	cin >> queryString;
	vector <string> options;
	for (auto x : fileList)
	{
		// check if queryString is a prefix
		if (x.length() >= queryString.length() && x.substr(0, queryString.length()) == queryString)
		{
			options.push_back(x);
		}
	}
	if (options.size() == 1)
	{
		cout << options[0] << endl;
	}
	else
	{
		int count = 0;
		for (auto x : options)
		{
			cout << ++count << ") " << x << endl; 
		}
		int choice = 0;
		cin >> choice;
		if (choice > 0 && choice <= options.size())
		{
			cout << options[choice - 1] << endl;
		}
		else
		{
			cout << "wtf" << endl;
		}
	}
}