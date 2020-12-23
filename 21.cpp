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

template<typename T, typename U>
struct TwoWayMap
{
	map<T, U> forward;
	map<U, T> backward;

	void Add(const pair<T, U>& p)
	{
		forward.insert(p);
		backward.insert(make_pair(p.second, p.first));
	}

	void Add(const T& t, const U& u)
	{
		forward.insert(make_pair(t,u));
		backward.insert(make_pair(u,t));
	}

	bool HasKey(const T& t)
	{
		return forward.find(t) != forward.end();
	}

	size_t Size() const { return forward.size(); }

};


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
				scores(y, x) = int(2*N);
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

struct Foods
{
	TwoWayMap<string, int> ingr_names, allerg_names;
	TwoWayMap<string, int> potential_ingrs;
	vector<vector<int>> ingr_ids, allerg_ids;

	void AddFoodParts(vector<string> ingrs, vector<string> allergs)
	{
		for (auto ingr : ingrs)
			AddIngredient(ingr);
		for (auto allerg : allergs)
			AddAllergen(allerg);
	}

	void AddFood(vector<string> ingrs, vector<string> allergs)
	{
		vector<int> ingr_id(ingr_names.Size(), 0);
		for (auto ingr : ingrs)
			ingr_id[ingr_names.forward[ingr]] = 1;
		ingr_ids.push_back(ingr_id);

		vector<int> allerg_id(allerg_names.Size(), 0);
		for (auto allerg : allergs)
			allerg_id[allerg_names.forward[allerg]] = 1;
		allerg_ids.push_back(allerg_id);
	}

	void Solve()
	{
		Tensor2D<int> allerg_eligibility(allerg_ids[0].size(), allerg_ids[0].size());

		int bads = 0;
		for (int ingr_i = 0; ingr_i < ingr_names.Size(); ++ingr_i)
		{
			int total_fits = 0;
			for (int allerg_i = 0; allerg_i < allerg_names.Size(); ++allerg_i)
			{
				bool fits = true;

				for (int i = 0; i < ingr_ids.size(); ++i)
				{
					bool first = ingr_ids[i][ingr_i], second = allerg_ids[i][allerg_i];

					bool result = !(!first && second);
					if (!result)
						fits = false;
				}
				if (fits)
				{
					++total_fits;

					string potential_name = ingr_names.backward[ingr_i];

					if (!potential_ingrs.HasKey(potential_name))
						potential_ingrs.Add(potential_name, potential_ingrs.Size());

					//cout << potential_name << potential_ingrs.forward[potential_name] << " fits with " << allerg_names.backward[allerg_i] << allerg_i << endl;

					allerg_eligibility(potential_ingrs.forward[potential_name], allerg_i) = 1;
				}
			}
			if (total_fits == 0)
			{
				for (int i = 0; i < ingr_ids.size(); ++i)
				{
					bads += ingr_ids[i][ingr_i];
				}
			}
		}

		cout << "Part 1: " << bads << endl;

		auto result = GeneralizedHamiltonian(allerg_eligibility);

		TwoWayMap<int, int> ingr_to_allerg;
		for (auto shape : result)
			ingr_to_allerg.Add(shape.Y, shape.X);

		vector<pair<string, string>> results;

		for (int i = 0; i < ingr_to_allerg.Size(); ++i)
		{
			int j = ingr_to_allerg.forward[i];

			results.push_back(make_pair(allerg_names.backward[j], potential_ingrs.backward[i]));
		}

		std::sort(results.begin(), results.end());

		cout << "Part 2: ";

		bool first = true;
		for (auto r : results)
		{
			if (first)
				first = false;
			else
				cout << ",";
			cout << r.second;
		}
		cout << endl;
	}

	void AddIngredient(string ingr)
	{
		if (!ingr_names.HasKey(ingr))
		{
			int value = ingr_names.Size();
			ingr_names.Add(ingr, value);
		}
	}
	void AddAllergen(string allerg)
	{
		if (!allerg_names.HasKey(allerg))
		{
			int value = allerg_names.Size();
			allerg_names.Add(allerg, value);
		}
	}
};

int main()
{
	Foods foods;

	auto lines = GetLines("21.txt");

	for (auto& line : lines)
	{
		auto parts = split(line.substr(0, line.length() - 1), " (contains ");

		if (parts.size() != 2)
		{
			cout << "Line malformed: " << line << endl;
			return -1;
		}

		auto ingrs = split(parts[0], " ");
		auto allergs = split(parts[1], ", ");
		foods.AddFoodParts(ingrs, allergs);
	}
	for (auto& line : lines)
	{
		auto parts = split(line.substr(0, line.length() - 1), " (contains ");
		auto ingrs = split(parts[0], " ");
		auto allergs = split(parts[1], ", ");
		foods.AddFood(ingrs, allergs);
	}

	foods.Solve();
}
