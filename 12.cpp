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

struct Instruction
{
	char opcode;
	int value;

	explicit Instruction(string s)
	{
		opcode = s[0];
		value = FromString<int>(s.substr(1));
	}
};

struct Coord2D
{
	int x=0,y=0;
};

int main()
{
	auto lines = GetLines("12.txt");

	vector<Instruction> instructions;
	for(auto& line: lines)
		instructions.push_back(Instruction(line));

	{
		//using +X = east, +Y = north, default rotation direction *clockwise*
		Coord2D position{ 0,0 };
		vector<Coord2D> headings =
		{
			{1,0},
			{0,-1},
			{-1,0},
			{0,1},
		};
		unsigned int heading = 0;

		for (const Instruction& instr : instructions)
		{
			if (instr.opcode == 'N')
				position.y += instr.value;
			if (instr.opcode == 'S')
				position.y -= instr.value;
			if (instr.opcode == 'E')
				position.x += instr.value;
			if (instr.opcode == 'W')
				position.x -= instr.value;
			if (instr.opcode == 'L')
				heading -= instr.value / 90;
			if (instr.opcode == 'R')
				heading += instr.value / 90;
			if (instr.opcode == 'F')
			{
				position.x += headings[heading % 4].x * instr.value;
				position.y += headings[heading % 4].y * instr.value;
			}
		}
		cout << "Part 1: " << abs(position.x) + abs(position.y) << endl;
	}

	{
		//using +X = east, +Y = north, default rotation direction *clockwise*
		Coord2D ship_pos{ 0,0 }, waypoint_pos{ 10, 1 };
		vector<Coord2D> headings =
		{
			{1,0},
			{0,-1},
			{-1,0},
			{0,1},
		};

		for (const Instruction& instr : instructions)
		{
			if (instr.opcode == 'N')
				waypoint_pos.y += instr.value;
			if (instr.opcode == 'S')
				waypoint_pos.y -= instr.value;
			if (instr.opcode == 'E')
				waypoint_pos.x += instr.value;
			if (instr.opcode == 'W')
				waypoint_pos.x -= instr.value;
			//handle both rotations with single code
			if (instr.opcode == 'L' || instr.opcode == 'R')
			{
				int val = instr.value;
				val /= 90;
				if (instr.opcode == 'L')
					val = (4-val)%4;

				//do *val* 90 degree CW rotations
				for (int i = 0; i < val; ++i)
				{
					std::swap(waypoint_pos.x, waypoint_pos.y);
					waypoint_pos.y = -waypoint_pos.y;
				}

			}
			if (instr.opcode == 'F')
			{
				ship_pos.x += waypoint_pos.x * instr.value;
				ship_pos.y += waypoint_pos.y * instr.value;
			}
		}
		cout << "Part 2: " << abs(ship_pos.x) + abs(ship_pos.y) << endl;
	}
}
