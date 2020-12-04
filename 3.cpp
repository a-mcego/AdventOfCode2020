#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>

using namespace std;

#define deb(x) cout << #x << ": " << ((x)) << endl;

template<typename T>
T FromString(const string& str)
{
	T t;
	stringstream ss(str);
	ss >> t;
	return t;
}
template<typename T>
string ToString(const T& t)
{
	stringstream ss;
	ss << t;
	return ss.str();
}

vector<string> split(const string& str, const string& delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos) pos = str.length();
		string token = str.substr(prev, pos - prev);
		tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

int CountCharInString(const string& str, char c)
{
	int count = 0;
	for (char c2 : str)
	{
		if (c2 == c)
			++count;
	}
	return count;
}

vector<string> GetLines(string filename)
{
	ifstream file(filename);
	vector<string> ret;
	while (true)
	{
		string str;
		std::getline(file, str);
		if (file.eof())
			break;
		ret.push_back(str);
	}
	return ret;
}

int main()
{
	auto lines = GetLines("3.txt");

	int linelength = lines[0].size();

	{
		int pos_x = 0, pos_y = 0;

		int delta_x = 3, delta_y = 1;

		int trees = 0;
		for (int pos_y = 0; pos_y < lines.size(); pos_y += delta_y)
		{
			if (lines[pos_y][pos_x] == '#')
				++trees;
			pos_x = (pos_x + delta_x) % linelength;
		}
		cout << "Part 1: " << trees << " trees found." << endl;

	}

	vector<pair<int,int>> deltas{{{1,1},{3,1},{5,1},{7,1},{1,2}}};

	long long int result = 1;

	for (auto& delta : deltas)
	{
		int delta_x = delta.first, delta_y = delta.second;

		int trees = 0, pos_x = 0;
		for (int pos_y = 0; pos_y < lines.size(); pos_y += delta_y)
		{
			if (lines[pos_y][pos_x] == '#')
				++trees;
			pos_x = (pos_x + delta_x) % linelength;
		}
		result *= trees;

	}
	cout << "Part 2: " << result << "." << endl;
}
