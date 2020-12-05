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

template<typename From, typename To>
struct Transformer
{
	map<From,To> m;

	void Add(const From& f, const To& t)
	{
		if (m.find(f) != m.end())
		{
			cout << "Warning: key already exists." << endl;
		}
		m[f] = t;
	}

	template<typename FromContainer>
	vector<To> Transform(FromContainer fc)
	{
		vector<To> tc;
		for (auto f: fc)
		{
			auto iter = m.find(f);
			if (iter == m.end())
			{
				cout << "Transformation doesn't cover the whole input set" << endl;
				return vector<To>();
			}
			tc.push_back(iter->second);
		}
		return tc;
	}
};

int main()
{
	auto lines = GetLines("5.txt");

	Transformer<char,int> tf;
	tf.Add('L', 0);
	tf.Add('R', 1);
	tf.Add('F', 0);
	tf.Add('B', 1);

	set<int> all_seat_numbers;

	for (auto line : lines)
	{
		auto numvec = tf.Transform(line);

		int num=0;
		for (auto n : numvec)
			num = num*2+n;

		all_seat_numbers.insert(num);
	}

	//highest seat number is the last item of the set
	{
		auto iter = all_seat_numbers.end();
		--iter;
		int highest_seat_number = *iter;
		cout << "Highest seat number: " << highest_seat_number << endl;
	}
	
	//the missing seat is our own seat
	{
		int previous = *all_seat_numbers.begin();
		auto iter = all_seat_numbers.begin();
		++iter;

		for (; iter != all_seat_numbers.end(); ++iter)
		{
			if (*iter - previous == 2)
				cout << "My seat is: " << previous+1 << endl;
			previous = *iter;
		}
	}
}
