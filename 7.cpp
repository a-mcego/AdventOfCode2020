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

struct CaseChild
{
	string name;
	int amount=-1;
};

struct CaseType
{
	string name;
	bool contains_shiny_gold = false;
	vector<CaseChild> children;
};

template<typename T>
struct Graph
{
	map<string,T> nodes;
	map<string,vector<string>> to;//connections to the key from the values
};

int main()
{
	auto lines = GetLines("7.txt");

	Graph<CaseType> case_graph;

	for (auto line : lines)
	{
		CaseType ct;

		auto c_split = split(line," contain ");

		ct.name = c_split[0].substr(0,c_split[0].length()-5); //remove " bags"

		vector<string> children_strings = split(c_split[1].substr(0,c_split[1].length()-1),", ");

		for (auto child : children_strings)
		{
			vector<string> child_pieces = split(child, " ");
			int start = child_pieces.front().length()+1;
			int end = child.length()-child_pieces.back().length()-1;
			string childname = child.substr(start, end - start);

			int amount = FromString<int>(child_pieces.front());

			CaseChild cc{childname, amount};

			ct.children.push_back(cc);

			case_graph.to[cc.name].push_back(ct.name);
		}
			
		case_graph.nodes[ct.name] = ct;
	}


	{
		queue<string> names;
		names.push("shiny gold");
		while (!names.empty())
		{
			string curr_name = names.front();
			vector<string> new_ones = case_graph.to[curr_name];

			case_graph.nodes[curr_name].contains_shiny_gold = true;

			names.pop();

			for (auto new_one : new_ones)
				names.push(new_one);
		}

		int shiny_golds = 0;
		for (auto& node : case_graph.nodes)
			if (node.first != "shiny gold" && node.second.contains_shiny_gold)
				++shiny_golds;

		cout << "Part 1: " << shiny_golds << " kinds of bag can contain shiny golds." << endl;
	}
	
	{
		queue<pair<string, int>> names;
		int total = 0;
		names.push(make_pair("shiny gold", 1));
		while (!names.empty())
		{
			pair<string, int> curr_thing = names.front();
			names.pop();

			if (curr_thing.first != "shiny gold")
				total += curr_thing.second;

			for (auto child: case_graph.nodes[curr_thing.first].children)
				names.push(make_pair(child.name,child.amount*curr_thing.second));
		}

		cout << "Part 2: " << total << " total amount of bags!" << endl;
	}
}
