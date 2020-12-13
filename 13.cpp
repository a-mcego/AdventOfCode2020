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
	auto lines = GetLines("13.txt");

	int start_time = FromString<int>(lines[0]);

	vector<string> bus_strings = split(lines[1],",");

	//Part 1
	{
		vector<int> bus_lines;
		for (auto bus_string : bus_strings)
			if (bus_string != "x")
				bus_lines.push_back(FromString<int>(bus_string));

		map<int, int> waittime_to_busline; //okay to use a map instead of multimap; there's always exactly one solution
		for (auto bus_line : bus_lines)
		{
			int wait_time = (bus_line - start_time % bus_line) % bus_line;
			waittime_to_busline[wait_time] = bus_line;
		}

		auto step1_result = *waittime_to_busline.begin();
		cout << "Part 1 result: " << step1_result.first * step1_result.second << endl;
	}

	//Part 2
	{
		//assume bus line numbers are prime, which implies theyre pairwise coprime
		//we can use chinese remainder theorem
		vector<pair<int64,int64>> bus_line_numbers;
		for (int bsi=0; bsi<bus_strings.size(); ++bsi)
		{
			auto bus_string = bus_strings[bsi];
			if (bus_string == "x")
				continue;

			int bus_line = FromString<int>(bus_string);

			//we have to take the additive inverse of the times
			bus_line_numbers.push_back(make_pair((bus_line-bsi%bus_line)%bus_line, bus_line));
		}

		//do chinese remainder theorem
		int64 current_number = 0, previous_coef = 1;
		for (auto pr : bus_line_numbers)
		{
			while (true)
			{
				if (current_number % pr.second == pr.first)
				{
					previous_coef *= pr.second;
					break;
				}
				current_number += previous_coef;
			}
		}
		cout << "Part 2 result: " << current_number << endl;
	}
}
