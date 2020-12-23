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
#include <tuple>

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

template<typename From, typename To>
void Transform(const vector<From>& input, vector<To>& output, To(*const func)(const From&))
{
	output.assign(input.size(), To());
	std::transform(input.begin(), input.end(), output.begin(), func);
}

template<typename From, typename To>
vector<To> Transform(const vector<From>& input, To(*const func)(const From&))
{
	vector<To> output(input.size());
	std::transform(input.begin(), input.end(), output.begin(), func);
	return output;
}


struct Triplet
{
	int x = 0, y = 0, z = 0;

	std::strong_ordering operator<=>(const Triplet& rhs) const = default;

	Triplet operator+(const Triplet& rhs) const
	{
		Triplet ret;
		ret.x = x + rhs.x;
		ret.y = y + rhs.y;
		ret.z = z + rhs.z;
		return ret;
	}
};
struct Quadruplet
{
	int x = 0, y = 0, z = 0, w = 0;

	std::strong_ordering operator<=>(const Quadruplet& rhs) const = default;

	Quadruplet operator+(const Quadruplet& rhs) const
	{
		Quadruplet ret;
		ret.x = x + rhs.x;
		ret.y = y + rhs.y;
		ret.z = z + rhs.z;
		ret.w = w + rhs.w;
		return ret;
	}
};

int main()
{
	auto lines = GetLines("17.txt");

	{
		set<Triplet> state;

		for (int l = 0; l < lines.size(); ++l)
			for (int c = 0; c < lines[l].size(); ++c)
				if (lines[l][c] == '#')
					state.insert(Triplet{ c,l,0 });

		for (int iter = 0; iter < 6; ++iter)
		{
			map<Triplet, int> neighbors;

			for (auto& tr : state)
			{
				for (int zdelta = -1; zdelta <= 1; ++zdelta)
				{
					for (int ydelta = -1; ydelta <= 1; ++ydelta)
					{
						for (int xdelta = -1; xdelta <= 1; ++xdelta)
						{
							if (zdelta == 0 && ydelta == 0 && xdelta == 0)
								continue;
							neighbors[tr + Triplet{ xdelta,ydelta,zdelta }] += 1;
						}
					}
				}
			}

			set<Triplet> new_state;
			for (auto& nb : neighbors)
			{
				if (nb.second == 3)
					new_state.insert(nb.first);
				if (nb.second == 2 && state.find(nb.first) != state.end())
					new_state.insert(nb.first);
			}
			state = new_state;
		}
		cout << "Part 1: " << state.size() << endl;
	}

	{
		set<Quadruplet> state;

		for (int l = 0; l < lines.size(); ++l)
			for (int c = 0; c < lines[l].size(); ++c)
				if (lines[l][c] == '#')
					state.insert(Quadruplet{ c,l,0,0 });

		for (int iter = 0; iter < 6; ++iter)
		{
			map<Quadruplet, int> neighbors;

			for (auto& tr : state)
			{
				for (int wdelta = -1; wdelta <= 1; ++wdelta)
				{
					for (int zdelta = -1; zdelta <= 1; ++zdelta)
					{
						for (int ydelta = -1; ydelta <= 1; ++ydelta)
						{
							for (int xdelta = -1; xdelta <= 1; ++xdelta)
							{
								if (zdelta == 0 && ydelta == 0 && xdelta == 0 && wdelta == 0)
									continue;
								neighbors[tr + Quadruplet{ xdelta,ydelta,zdelta,wdelta }] += 1;
							}
						}
					}
				}
			}

			set<Quadruplet> new_state;
			for (auto& nb : neighbors)
			{
				if (nb.second == 3)
					new_state.insert(nb.first);
				if (nb.second == 2 && state.find(nb.first) != state.end())
					new_state.insert(nb.first);
			}
			state = new_state;
		}
		cout << "Part 2: " << state.size() << endl;
	}
}
