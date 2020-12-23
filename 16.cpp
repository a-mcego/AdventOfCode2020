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
void Transform(const vector<From>& input, vector<To>& output, To(*func)(const From&))
{
	output.assign(input.size(), To());
	std::transform(input.begin(), input.end(), output.begin(), func);
}

template<typename From, typename To>
vector<To> Transform(const vector<From>& input, To(*func)(const From&))
{
	vector<To> output(input.size());
	std::transform(input.begin(), input.end(), output.begin(), func);
	return output;
}


struct Shape2D
{
	size_t Y, X;
};

template<typename T>
struct Tensor2D
{
private:
	vector<T> data;
public:
	const Shape2D shape;

	Tensor2D(size_t ysize, size_t xsize) :shape{ ysize,xsize }
	{
		data.assign(shape.X * shape.Y, 0);
	}

	T& operator()(size_t y, size_t x)
	{
		return data[y * shape.X + x];
	}
	const T& operator()(size_t y, size_t x) const
	{
		return data[y * shape.X + x];
	}
};

template<typename T>
ostream& operator<<(ostream& o, const Tensor2D<T>& t)
{
	for (int y = 0; y < t.shape.Y; ++y)
	{
		for (int x = 0; x < t.shape.X; ++x)
			o << t(y, x) << " ";
		o << endl;
	}
	return o;
}

ostream& operator<<(ostream& o, const Shape2D& t)
{
	o << "(" << t.Y << "," << t.X << ")";
	return o;
}



Tensor2D<int> GHScore(const Tensor2D<int>& t)
{
	if (t.shape.Y != t.shape.X)
	{
		cout << __FUNCTION__ << ": Input was not a square matrix." << endl;
		throw "";
	}

	size_t N = t.shape.Y;
	Tensor2D<int> scores(N, N), scores_y(N, 1), scores_x(1, N);

	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
			scores_y(y, 0) += t(y, x);
			scores_x(0, x) += t(y, x);
		}
	}

	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
			if (t(y, x))
				scores(y, x) = scores_y(y, 0) + scores_x(0, x) - 1;
			else
				scores(y, x) = int(2 * N);
		}
	}

	return scores;
}

vector<Shape2D> OrderByScore(const Tensor2D<int>& values, const Tensor2D<int>& scores)
{
	vector<pair<int, Shape2D>> all;

	for (size_t y = 0; y < values.shape.Y; ++y)
	{
		for (size_t x = 0; x < values.shape.X; ++x)
		{
			if (!values(y, x))
				continue;
			all.push_back(make_pair(scores(y, x), Shape2D{ y,x }));
		}
	}

	std::sort(all.begin(), all.end(), [](auto& lhs, auto& rhs) {return lhs.first < rhs.first; });

	vector<Shape2D> ret = Transform<pair<int, Shape2D>, Shape2D>(all, [](const pair<int, Shape2D>& pr) -> Shape2D { return pr.second; });

	return ret;
}

bool GH_recurse(vector<Shape2D>& result, const Tensor2D<int>& values)
{
	if (result.size() == values.shape.X)
		return true;

	Tensor2D<int> scores = GHScore(values);
	vector<Shape2D> best = OrderByScore(values, scores);

	for (Shape2D shape : best)
	{
		result.push_back(shape);
		//write zeros to the column and row we chose
		auto new_values = values;
		for (int x = 0; x < values.shape.X; ++x)
			new_values(shape.Y, x) = 0;
		for (int y = 0; y < values.shape.Y; ++y)
			new_values(y, shape.X) = 0;

		if (GH_recurse(result, new_values))
			return true;
		result.pop_back();
	}

	return false;
}

/*
Solves the "Generalized Hamiltonian" problem:
1) edges are directed
2) vertexes are allowed to have edges to itself
and most importantly
3) we're finding an arbitrary amount of cycles
   such that every vertex is part of exactly 1 cycle
*/
vector<Shape2D> GeneralizedHamiltonian(const Tensor2D<int>& t)
{
	vector<Shape2D> result;
	bool success = GH_recurse(result, t);
	if (!success)
		return vector<Shape2D>();
	return result;
}



int main()
{
	auto lines = GetLines("16.txt");

	enum struct State
	{
		CLASSES, YOURS, NEARBY
	} state = State::CLASSES;

	struct Column
	{
		string name;

		struct Range
		{
			int start=-1, end=-1;
		};

		vector<Range> ranges;

		bool IsInRange(int n)
		{
			for (auto& range : ranges)
			{
				if (range.start <= n && n <= range.end)
					return true;
			}
			return false;
		}
	};

	vector<vector<int>> nearby_tickets;
	vector<int> your_ticket;

	vector<Column> columns;

	for (auto line : lines)
	{
		if (line == "")
		{
			if (state == State::CLASSES)
				state = State::YOURS;
			else if (state == State::YOURS)
				state = State::NEARBY;
			continue;
		}
		if (state == State::CLASSES)
		{
			Column column;

			auto splitted = split(line,": ");
			column.name = splitted[0];

			auto range_strs = split(splitted[1]," or ");

			for(auto range_str : range_strs)
			{
				auto values = split(range_str, "-");
				Column::Range range;
				range.start = FromString<int>(values[0]);
				range.end = FromString<int>(values[1]);
				column.ranges.push_back(range);
			}
			columns.push_back(column);
		}
		else if (state == State::YOURS)
		{
			if (line == "your ticket:")
				continue;
			your_ticket = Transform(split(line, ","), FromString<int>);
		}
		else if (state == State::NEARBY)
		{
			if (line == "nearby tickets:")
				continue;
			vector<int> numbers = Transform(split(line, ","), FromString<int>);
			nearby_tickets.push_back(numbers);
		}
	}

	vector<bool> valid_values(1000,false);
	for (Column& column : columns)
	{
		for (auto& range : column.ranges)
		{
			for(int i=range.start; i<=range.end; ++i)
				valid_values[i] = true;
		}
	}

	vector<vector<int>> valid_tickets;
	valid_tickets.push_back(your_ticket);

	int bad_sum=0;
	for (vector<int>& line : nearby_tickets)
	{
		bool good_ticket = true;
		for (int n : line)
			if (!valid_values[n])
				bad_sum += n, good_ticket = false;
		if (good_ticket)
			valid_tickets.push_back(line);
	}

	cout << "Part 1: " << bad_sum << endl;

	Tensor2D<int> eligibles(columns.size(), columns.size());
	for (int c_id = 0; c_id < columns.size(); ++c_id)
	{
		Column& column = columns[c_id];
		for (int tc_id = 0; tc_id < columns.size(); ++tc_id)
		{
			int valids = 0;
			for (int ticket_id = 0; ticket_id < valid_tickets.size(); ++ticket_id)
			{
				auto& value = valid_tickets[ticket_id][tc_id];
				if (column.IsInRange(value))
					++valids;
			}
			if (valids == valid_tickets.size())
				eligibles(c_id, tc_id) = 1;
		}
	}

	auto pairs = GeneralizedHamiltonian(eligibles);

	vector<int> departure_columns;
	for (int cid = 0; cid < columns.size(); ++cid)
	{
		if (columns[cid].name.substr(0,9) == "departure")
			departure_columns.push_back(cid);
	}

	std::sort(pairs.begin(), pairs.end(), [](const Shape2D& lhs, const Shape2D& rhs) -> bool { return lhs.Y < rhs.Y; });

	int64 product = 1;
	for (auto c : departure_columns)
		product *= your_ticket[pairs[c].X];


	cout << "Part 2: " << product << endl;

}
