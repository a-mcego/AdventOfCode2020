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
	auto lines = GetLines("9.txt");
	const int preamble_size = 25;

	vector<int64> numbers;
	for(auto& line: lines)
		numbers.push_back(FromString<int64>(line));

	int64 find_sum = 0;

	for (int i = preamble_size; i < numbers.size(); ++i)
	{
		bool number_good = false;
		for (int first = i - preamble_size; first < i; ++first)
		{
			for (int second = first+1; second < i; ++second)
			{
				if (numbers[first]+numbers[second] == numbers[i])
					number_good = true;
			}
		}
		if (!number_good)
		{
			find_sum = numbers[i];
			break;
		}
	}

	cout << "Part 1: number found: " << find_sum << endl;

	vector<int64> cum_sums;
	cum_sums.push_back(0);
	for(int64 n: numbers)
		cum_sums.push_back(cum_sums.back()+n);

	int64 min_num = 0, max_num = 0;
	for(int first_position=0; first_position<cum_sums.size()-1; ++first_position)
	{
		int64 sum = cum_sums[first_position];
		auto range = std::equal_range(cum_sums.begin(), cum_sums.end(), find_sum+sum);
		if (range.first != range.second)
		{
			int last_position = range.first-cum_sums.begin()-1;

			min_num = *std::min_element(numbers.begin() + first_position, numbers.begin() + last_position);
			max_num = *std::max_element(numbers.begin() + first_position, numbers.begin() + last_position);
			break;
		}

	}
	cout << "Part 2: sum found: " << min_num + max_num << endl;
}
