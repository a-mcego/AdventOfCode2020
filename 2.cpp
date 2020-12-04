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
		if (c2==c)
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

struct Data
{
	Data(string str)
	{
		vector<string> strs = split(str, " ");

		vector<string> nums = split(strs[0], "-");

		start = FromString<int>(nums[0]);
		end = FromString<int>(nums[1]);

		ch = strs[1][0];
		password = strs[2];

		int count = CountCharInString(password, ch);

		is_good_1 = (count >= start && count <= end);
		is_good_2 = ((password[start - 1] == ch) ^ (password[end - 1] == ch));
	}

	int start = -1, end = -1;
	char ch = ' ';
	string password;

	bool is_good_1, is_good_2;
};

Data GetData(string str)
{
	Data data = Data(str);
	return data;
}

int main()
{
	auto lines = GetLines("2.txt");

	int value_1 = 0;
	for (string s : lines)
	{
		if (Data(s).is_good_1)
			++value_1;
	}
	int value_2 = 0;
	for (string s : lines)
	{
		if (Data(s).is_good_2)
			++value_2;
	}

	cout << "Part 1: " << value_1 << endl;
	cout << "Part 2: " << value_2 << endl;
}
