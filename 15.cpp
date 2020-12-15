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

struct NumberMemory
{
	int64 curr_step = 1;
	vector<int64> last_step;

	int64 Add(int64 n)
	{
		if (last_step.size() < n+1)
			last_step.resize(n+1, -1);
		if (last_step[n] == -1)
		{
			last_step[n] = curr_step;
			++curr_step;
			return 0;
		}
		int64 last_step_value = last_step[n];
		int64 ret = curr_step - last_step_value;
		last_step[n] = curr_step;
		++curr_step;
		return ret;
	}
};

int main()
{
	auto lines = GetLines("15.txt");

	auto val_strings = split(lines[0], ",");
	vector<int64> values(val_strings.size());

	std::transform(val_strings.begin(), val_strings.end(), values.begin(), FromString<int64>);

	NumberMemory numbermemory;

	int64 next_n=0;
	for (int i=0; i<values.size(); ++i)
	{
		next_n = numbermemory.Add(values[i]);
	}

	while (true)
	{
		next_n = numbermemory.Add(next_n);
		if (numbermemory.curr_step == 2020)
		{
			cout << "Step 1: " << next_n << endl;
		}
		if (numbermemory.curr_step == 30000000)
		{
			cout << "Step 2: " << next_n << endl;
			break;
		}
	}
}
