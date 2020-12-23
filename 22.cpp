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
#include <regex>
#include <stack>

using namespace std;

using int64 = int64_t;

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
void Transform(const vector<From>& input, vector<To>& output, To(* const func)(const From&))
{
	output.assign(input.size(), To());
	std::transform(input.begin(), input.end(), output.begin(), func);
}

template<typename From, typename To>
vector<To> Transform(const vector<From>& input, To(* const func)(const From&))
{
	vector<To> output(input.size());
	std::transform(input.begin(), input.end(), output.begin(), func);
	return output;
}

ostream& operator<<(ostream& o, const deque<int>& d)
{
	for (auto di : d)
		cout << di << " ";
	return o;
}

pair<int,deque<int>> RecursiveGame(deque<int> p1, deque<int> p2)
{
	set<pair<deque<int>, deque<int>>> memory;

	while(true)
	{
		if (p1.empty())
		{
			return make_pair(2,p2);
		}
		if (p2.empty())
		{
			return make_pair(1,p1);
		}
		if (memory.find(make_pair(p1, p2)) != memory.end())
		{
			return make_pair(1, deque<int>());
		}
		memory.insert(make_pair(p1, p2));

		int p1_card = p1.front(); p1.pop_front();
		int p2_card = p2.front(); p2.pop_front();

		int winner = 0;

		if (p1_card <= p1.size() && p2_card <= p2.size())
		{
			winner = RecursiveGame(deque<int>(p1.begin(),p1.begin()+p1_card), deque<int>(p2.begin(),p2.begin()+p2_card)).first;
		}
		else
		{
			if (p1_card > p2_card)
				winner = 1;
			else
				winner = 2;
		}

		if (winner == 1)
		{ 
			p1.push_back(p1_card);
			p1.push_back(p2_card);
		}
		else if (winner == 2)
		{
			p2.push_back(p2_card);
			p2.push_back(p1_card);
		}
		else
		{
			cout << "Winner was " << winner << "?" << endl;
		}
	}
}

int main()
{
	auto lines = GetLines("22.txt");

	deque<int> p1_start, p2_start;

	{
		deque<int>* qptr = &p1_start;
		for (auto& line : lines)
		{
			if (line == "Player 1:")
			{
				qptr = &p1_start;
				continue;
			}
			else if (line == "Player 2:")
			{
				qptr = &p2_start;
				continue;
			}
			else if (line.empty())
			{
				continue;
			}
			qptr->push_back(FromString<int>(line));
		}
	}

	deque<int> win_queue, p1=p1_start, p2=p2_start;

	int moves = 0;
	while (true)
	{
		++moves;
		if (p1.empty())
		{
			win_queue = p2;
			break;
		}
		if (p2.empty())
		{
			win_queue = p1;
			break;
		}

		int p1_card = p1.front(); p1.pop_front();
		int p2_card = p2.front(); p2.pop_front();
		if (p1_card > p2_card)
		{
			p1.push_back(p1_card);
			p1.push_back(p2_card);
		}
		else
		{
			p2.push_back(p2_card);
			p2.push_back(p1_card);
		}
	}

	{
		int64 sum = 0;
		for (int i = 0; i < win_queue.size(); ++i)
			sum += (win_queue.size() - i) * win_queue[i];

		cout << "Part 1: " << sum << endl;

	}

	auto result = RecursiveGame(p1_start, p2_start);

	{
		int64 sum = 0;
		for (int i = 0; i < result.second.size(); ++i)
			sum += (result.second.size() - i) * result.second[i];

		cout << "Part 2: " << sum << endl;

	}
}
