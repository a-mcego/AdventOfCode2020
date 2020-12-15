#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
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
	auto lines = GetLines("14.txt");

	{
		unordered_map<int64, int64> values;
		int64 mask = 0, maskvalue = 0;

		for (auto line : lines)
		{
			auto parts = split(line," = ");

			auto instruction = parts[0], value = parts[1];
			if (instruction.substr(0, 4) == "mask")
			{
				mask = 0, maskvalue = 0;
				for (const char c : value)
				{	
					mask <<= 1, maskvalue <<= 1;
					if (c == 'X')
					{
						mask |= 1LL;
					}
					else if (c == '0'); //nothing needs to be done
					else if (c == '1')
					{
						maskvalue |= 1LL;
					}
				}
			}
			else if (instruction.substr(0, 4) == "mem[")
			{
				int64 mem_address = FromString<int64>(instruction.substr(4,instruction.length()-5));
				int64 mem_value = ((FromString<int64>(value) & mask) | maskvalue);
				values[mem_address] = mem_value;
			}
			else
			{
				cout << "WEIRD INPUT." << endl;
				return -1;
			}
		}

		int64 sum = 0;
		for (auto addr_value_pair : values)
			sum += addr_value_pair.second;
		cout << "Part 1: " << sum << endl;
	}


	{
		unordered_map<int64, int64> values;
		int64 maskvalue = 0;
		vector<int> mask_Xs;

		for (auto line : lines)
		{
			auto parts = split(line, " = ");

			auto instruction = parts[0], value = parts[1];
			if (instruction.substr(0, 4) == "mask")
			{
				maskvalue = 0;
				mask_Xs.clear();
				int position = 35;
				for (const char c : value)
				{
					maskvalue <<= 1;
					if (c == 'X')
					{
						mask_Xs.push_back(position);
					}
					else if (c == '0'); //nothing needs to be done
					else if (c == '1')
					{
						maskvalue |= 1LL;
					}
					--position;
				}
			}
			else if (instruction.substr(0, 4) == "mem[")
			{
				int64 base_mem_address = FromString<int64>(instruction.substr(4, instruction.length() - 5)) | maskvalue;
				int64 mem_value = FromString<int64>(value);

				for (int i = 0; i < (1LL << mask_Xs.size()); ++i)
				{
					int64 mem_address = base_mem_address;
					for (int X_id = 0; X_id < mask_Xs.size(); ++X_id)
					{
						int64 X_val = mask_Xs[X_id];

						mem_address &= ~(1LL<<X_val);
						mem_address |= (int64((i >> X_id) & 1) << X_val);
					}
					values[mem_address] = mem_value;
				}
			}
			else
			{
				cout << "WEIRD INPUT." << endl;
				return -1;
			}
		}

		int64 sum = 0;
		for (auto addr_value_pair : values)
			sum += addr_value_pair.second;
		cout << "Part 2: " << sum << ", total amount of memory addresses: " << values.size() << endl;
	}

}
