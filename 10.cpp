#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <cstdint>
#include <chrono>

using namespace std;

#define deb(x) cout << #x << ": " << ((x)) << endl;

long long int starttime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
long long int cloque()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - starttime;
}

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

using int64 = int64_t;

int main()
{
	auto lines = GetLines("10.txt");

	vector<int> numbers;
	for(auto& line: lines)
		numbers.push_back(FromString<int>(line));

	//wall
	numbers.push_back(0);

	std::sort(numbers.begin(),numbers.end());

	//the device itself
	numbers.push_back(numbers.back()+3);

	//part 1
	int ones=0,threes=0;
	for (int i = 1; i < numbers.size(); ++i)
	{
		int difference = numbers[i] - numbers[i - 1];
		if (difference == 1)
			++ones;
		else if (difference == 3)
			++threes;
	}
	cout << "Part 1: " << ones << "*" << threes << "=" << ones*threes << endl;

	//part 2
	//bottom-up linear time algorithm
	vector<int64> ways(numbers.size(), 0);
	ways.back() = 1;
	for (int i = ways.size() - 1; i > 0; --i)
	{
		for (int spread_to = i-1; spread_to >= 0; --spread_to)
		{
			if (numbers[i]-numbers[spread_to] > 3)
				break;
			ways[spread_to] += ways[i];
		}
	}
	cout << "Part 2: " << ways[0] << endl;
}
