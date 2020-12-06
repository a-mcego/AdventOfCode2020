#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>
#include <map>
#include <set>

using namespace std;

#define deb(x) cout << #x << ": " << ((x)) << endl;

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

struct Group
{
	map<char,int> answers;
	int people=0;
};

int main()
{
	auto lines = GetLines("6.txt");

	vector<Group> groups;
	Group g;
	for (auto line : lines)
	{
		if (line == "")
		{
			groups.push_back(g);
			g = Group();
		}
		else
		{
			for (auto c : line)
				++g.answers[c];
			++g.people;
		}
	}
	groups.push_back(g);

	int totalsum1 = 0, totalsum2 = 0;
	for (auto g : groups)
	{
		totalsum1 += (int)g.answers.size();
		for (auto answer : g.answers)
			totalsum2 += (g.people == answer.second ? 1 : 0);
	}
	
	cout << "Part 1: sum is " << totalsum1 << endl;
	cout << "Part 2: sum is " << totalsum2 << endl;
}
