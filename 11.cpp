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

struct Tensor2D
{
	vector<int> data;
	int X,Y;

	Tensor2D(int sizex, int sizey) :X(sizex), Y(sizey)
	{
		data.assign(X*Y, 0);
	}

	int& operator()(int x, int y)
	{
		if (!ValidCoord(x,y))
			cout << __FUNCTION__ << ": Warning, invalid coords used: " << x << "," << y << endl;
		return data[y*X+x];
	}

	bool operator==(const Tensor2D& rhs) const
	{
		return X == rhs.X && Y == rhs.Y && data == rhs.data;
	}

	int Count(int n)
	{
		int count=0;
		for(auto d: data)
			if (d==n)
				++count;
		return count;
	}

	bool ValidCoord(int x, int y)
	{
		return x >= 0 && x < X && y >= 0 && y < Y;
	}

	void Print()
	{
		for (int y = 0; y < Y; ++y)
		{
			for (int x = 0; x < X; ++x)
			{
				cout << char((*this)(x,y));
			}
			cout << endl;
		}
		cout << endl;
	}
};

int main()
{
	auto lines = GetLines("11.txt");

	Tensor2D original_grid(lines.back().size(), lines.size());

	for (int y = 0; y < original_grid.Y; ++y)
		for (int x = 0; x < original_grid.X; ++x)
			original_grid(x,y) = lines[y][x];

	{
		Tensor2D tensor = original_grid;

		while (true)
		{
			Tensor2D newtensor = tensor;
			for (int y = 0; y < tensor.Y; ++y)
			{
				for (int x = 0; x < tensor.X; ++x)
				{
					if (tensor(x, y) == '.')
						continue;

					int occupied_neighbors = 0;
					for (int ydelta = -1; ydelta <= 1; ++ydelta)
					{
						for (int xdelta = -1; xdelta <= 1; ++xdelta)
						{
							//cell itself is not a neighbor
							if (ydelta == 0 && xdelta == 0)
								continue;
							if (!tensor.ValidCoord(x + xdelta, y + ydelta))
								continue;
							if (tensor(x + xdelta, y + ydelta) == '#')
								++occupied_neighbors;
						}
					}

					if (tensor(x, y) == 'L')
					{
						if (occupied_neighbors == 0)
							newtensor(x, y) = '#';
					}
					else if (tensor(x, y) == '#')
					{
						if (occupied_neighbors >= 4)
							newtensor(x, y) = 'L';
					}
				}
			}

			bool should_break = (newtensor == tensor);
			tensor = newtensor;
			if (should_break)
				break;
		}
		cout << "Part 1: " << tensor.Count('#') << " seats occupied." << endl;

	}
	{
		Tensor2D tensor = original_grid;

		while (true)
		{
			Tensor2D newtensor = tensor;
			for (int y = 0; y < tensor.Y; ++y)
			{
				for (int x = 0; x < tensor.X; ++x)
				{
					if (tensor(x, y) == '.')
						continue;

					int occupied_neighbors = 0;
					for (int ydelta = -1; ydelta <= 1; ++ydelta)
					{
						for (int xdelta = -1; xdelta <= 1; ++xdelta)
						{
							//cell itself is not a neighbor
							if (ydelta == 0 && xdelta == 0)
								continue;
							for (int coef = 1;; ++coef)
							{
								if (!tensor.ValidCoord(x + xdelta * coef, y + ydelta * coef))
									break;
								int value = tensor(x + xdelta * coef, y + ydelta * coef);
								if (value == '#')
								{
									++occupied_neighbors;
									break;
								}
								if (value == 'L')
									break;
							}
						}
					}

					if (tensor(x, y) == 'L')
					{
						if (occupied_neighbors == 0)
							newtensor(x, y) = '#';
					}
					else if (tensor(x, y) == '#')
					{
						if (occupied_neighbors >= 5)
							newtensor(x, y) = 'L';
					}
				}
			}

			bool should_break = (newtensor == tensor);
			tensor = newtensor;
			if (should_break)
				break;
		}
		cout << "Part 2: " << tensor.Count('#') << " seats occupied." << endl;

	}
}
