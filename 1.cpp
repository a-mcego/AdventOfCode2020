#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>

using namespace std;

int main()
{
	vector<int> numbers;

	ifstream file("1.txt");
	while (true)
	{
		int n;
		file >> n;
		if (file.eof())
			break;
		numbers.push_back(n);
	}

	//PART 1
	std::sort(numbers.begin(), numbers.end());

	const int SUM = 2020;
	for (auto it = numbers.begin(); it!=numbers.end(); ++it)
	{
		int n = *it;
		if (std::binary_search(it + 1, numbers.end(), SUM - n))
		{
			cout << n << " * " << SUM - n << " = " << n * (SUM - n) << endl;
		}
	}

	//PART 2
	for (auto it = numbers.begin(); it != numbers.end(); ++it)
	{
		int a = *it;

		for (auto it2 = it+1; it2 != numbers.end(); ++it2)
		{
			int b = *it2;
			if (a+b+b >= SUM)
				break;

			if (std::binary_search(it2 + 1, numbers.end(), SUM - a - b))
			{
				cout << a << " * " << b << " * " << (SUM - a - b) << " = " << a * b * (SUM - a - b) << endl;
			}
		}
	}

}
