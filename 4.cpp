#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <string>
#include <sstream>
#include <map>

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

bool All(const vector<bool>& v)
{
	for (bool b : v)
		if (!b)
			return false;
	return true;
}
bool Any(const vector<bool>& v)
{
	for (bool b : v)
		if (b)
			return true;
	return false;
}

bool BetweenInclusive(int n, int low, int high)
{
	return (n >= low && n <= high);
}

bool IsNumeric(const string& s)
{
	for (char c : s)
	{
		if (!(c >= '0' && c <= '9'))
			return false;
	}
	return true;
}

bool IsHex(const string& s)
{
	for (char c : s)
	{
		if (!(c >= '0' && c <= '9' || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f'))
			return false;
	}
	return true;
}

template<typename T>
bool IsIn(const vector<T>& v, const T& t)
{
	for (const auto& val : v)
	{
		if (t==val)
			return true;
	}
	return false;
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

int CountCharInString(const string& str, char c)
{
	int count = 0;
	for (char c2 : str)
	{
		if (c2 == c)
			++count;
	}
	return count;
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

template<typename T>
struct Instance
{
private:
	bool initialized = false;
	T value;
public:
	void Set(const T& value_) { value = value_; initialized = true; }
	const T& Get() { return value; }
	bool Inited() { return initialized; }
	void Reset() { initialized = false; }
};

struct Passport
{
	map<string,Instance<string>> attrs;

	bool IsGood1()
	{
		//intentionally missing cid
		vector<string> check = { "byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid" };

		vector<bool> results;
		for (auto str : check)
			results.push_back(attrs[str].Inited());
		return All(results);
	}

	bool IsGood2()
	{
		if (!IsGood1())
			return false;
		{
			string str = attrs["byr"].Get();
			if (str.length() != 4)
				return false;
			if (!IsNumeric(str))
				return false;
			int intval = FromString<int>(str);
			if (!BetweenInclusive(intval, 1920, 2002))
				return false;
		}
		{
			string str = attrs["iyr"].Get();
			if (str.length() != 4)
				return false;
			if (!IsNumeric(str))
				return false;
			int intval = FromString<int>(str);
			if (!BetweenInclusive(intval, 2010, 2020))
				return false;
		}
		{
			string str = attrs["eyr"].Get();
			if (str.length() != 4)
				return false;
			if (!IsNumeric(str))
				return false;
			int intval = FromString<int>(str);
			if (!BetweenInclusive(intval, 2020, 2030))
				return false;
		}
		{
			string str = attrs["hgt"].Get();
			if (str.length() < 4)
				return false;
			string unit = str.substr(str.length() - 2);
			if (unit != "cm" && unit != "in")
				return false;
			string num = str.substr(0, str.length() - 2);
			if (!IsNumeric(num))
				return false;
			int intval = FromString<int>(num);
			if (unit == "cm")
				if (!BetweenInclusive(intval, 150,193))
					return false;
			if (unit == "in")
				if (!BetweenInclusive(intval, 59,76))
					return false;
		}
		{
			string str = attrs["hcl"].Get();
			if (str.length() != 7)
				return false;
			if (!IsHex(str.substr(1)))
				return false;
		}
		{
			vector<string> eyecolors = {"amb","blu","brn","gry","grn","hzl","oth"};
			string str = attrs["ecl"].Get();
			if (!IsIn(eyecolors, str))
				return false;
		}
		{
			string str = attrs["pid"].Get();
			if (str.length() != 9)
				return false;
			if (!IsNumeric(str))
				return false;
		}

		return true;
	}
};

int main()
{
	auto lines = GetLines("4.txt");

	int good_passports1 = 0, good_passports2 = 0;

	Passport p;

	for(auto line: lines)
	{
		if (line == "")
		{
			if (p.IsGood1())
				++good_passports1;
			if (p.IsGood2())
				++good_passports2;
			p = Passport();
		}
		else
		{
			auto parts = split(line, " ");
			for (auto part : parts)
			{
				auto kv = split(part, ":");
				p.attrs[kv[0]].Set(kv[1]);
			}
		}
	}
	if (p.IsGood1())
		++good_passports1;
	if (p.IsGood2())
		++good_passports2;

	cout << "Part 1: " << good_passports1 << " good passports found!" << endl;
	cout << "Part 2: " << good_passports2 << " good passports found!" << endl;
}
