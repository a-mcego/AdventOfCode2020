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

struct Token
{
	enum struct Type
	{
		PAREN_OPEN,
		PAREN_CLOSE,
		OPERATOR,
		LITERAL,
		WHITESPACE
	} type;
	string data;
};

ostream& operator<<(ostream& o, const Token::Type& t)
{
#define print_token_stuff(x) if (t==Token::Type::x) o << #x;
	print_token_stuff(PAREN_OPEN);
	print_token_stuff(PAREN_CLOSE);
	print_token_stuff(OPERATOR);
	print_token_stuff(LITERAL);
	print_token_stuff(WHITESPACE);
#undef print_token_stuff

	return o;
}

struct RegexMatcher
{
	Token::Type tokentype;
	string regex_string;
};

vector<Token> tokenize(const string& str)
{
	vector<Token> ret;

	const vector<RegexMatcher> token_regexes =
	{
		{Token::Type::LITERAL, "^[0-9]+"},
		{Token::Type::OPERATOR, "^(\\*|\\+||\\-)"},
		{Token::Type::PAREN_OPEN, "^\\("},
		{Token::Type::PAREN_CLOSE, "^\\)"},
		{Token::Type::WHITESPACE, "^(\\s)+"},
	};

	size_t position = 0;
	while (position < str.length())
	{
		string curr_str = str.substr(position);

		vector<Token> found_tokens;

		for (const auto& token_regex : token_regexes)
		{
			std::regex reg_obj(token_regex.regex_string);
			std::smatch matches;

			std::regex_search(curr_str, matches, reg_obj);

			Token new_token{ token_regex.tokentype, matches.str() };

			if (found_tokens.empty())
			{
				found_tokens.push_back(new_token);
			}
			else
			{
				if (found_tokens[0].data.length() < matches.length())
				{
					found_tokens.clear();
					found_tokens.push_back(new_token);
				}
				else if (found_tokens[0].data.length() == matches.length())
				{
					found_tokens.push_back(new_token);
				}
			}
		}

		if (found_tokens.empty())
		{
			cout << "Couldn't find tokens at string " << str << ", position " << position << endl;
			return ret;
		}
		if (found_tokens.size() > 1)
		{
			cout << "Ambiguous token at string " << str << ", position " << position << endl;
			return ret;
		}

		auto& found_token = found_tokens[0];
		ret.push_back(found_token);
		position += found_token.data.length();
	}


	return ret;
}

struct OperatorData
{
	enum struct Associativity { LEFT, RIGHT };
	int precedence;
	Associativity associativity;
};

map<string, OperatorData> operatordata_part1 =
{
	{"+", {1, OperatorData::Associativity::LEFT}},
	{"*", {1, OperatorData::Associativity::LEFT}},
};

map<string, OperatorData> operatordata_part2 =
{
	{"+", {2, OperatorData::Associativity::LEFT}},
	{"*", {1, OperatorData::Associativity::LEFT}},
};

//operatordata is not const because [] requires non-const and i'm way too lazy
//to rewrite this to use .find() or something similar
vector<Token> shunt(const vector<Token>& tokens, map<string, OperatorData>& operatordata)
{
	vector<Token> output;
	stack<Token> yard;

	for (const Token& token : tokens)
	{
		if (token.type == Token::Type::LITERAL)
			output.push_back(token);
		else if (token.type == Token::Type::OPERATOR)
		{
			while (!yard.empty() && 
				yard.top().type == Token::Type::OPERATOR && (
					operatordata[yard.top().data].precedence > operatordata[token.data].precedence
					||
					(operatordata[token.data].associativity == OperatorData::Associativity::LEFT 
					&& operatordata[yard.top().data].precedence >= operatordata[token.data].precedence)
				))
			{
				output.push_back(yard.top());
				yard.pop();
			}
			yard.push(token);
		}
		else if (token.type == Token::Type::PAREN_OPEN)
			yard.push(token);
		else if (token.type == Token::Type::PAREN_CLOSE)
		{
			if (yard.empty())
			{
				cout << "Mismatched parens." << endl;
				return output;
			}
			while (yard.top().type != Token::Type::PAREN_OPEN)
			{
				output.push_back(yard.top());
				yard.pop();
				if (yard.empty())
				{
					cout << "Mismatched parens." << endl;
					return output;
				}
			}
			yard.pop();
		}
	}
	while (!yard.empty() && yard.top().type == Token::Type::OPERATOR)
	{
		output.push_back(yard.top());
		yard.pop();
	}
	return output;
}

int64 rpn(const vector<Token>& rpn_tokens)
{
	stack<int64> rpn_stack;
	for (auto token : rpn_tokens)
	{
		if (token.type == Token::Type::LITERAL)
			rpn_stack.push(FromString<int64>(token.data));
		else if (token.type == Token::Type::OPERATOR)
		{
			int64 second_param = rpn_stack.top();
			rpn_stack.pop();
			int64 first_param = rpn_stack.top();
			rpn_stack.pop();

			if (token.data == "*")
				rpn_stack.push(first_param * second_param);
			else if (token.data == "+")
				rpn_stack.push(first_param + second_param);
			else
				cout << "Weird operator data: \"" << token.data << "\"." << endl;

			//cout << "Doing " << first_param << token.data << second_param << " = " << rpn_stack.top() << endl;
		}
	}
	return rpn_stack.top();
}

int main()
{
	auto lines = GetLines("18.txt");

	int64 resultsum_part1 = 0;
	for (auto& line : lines)
	{
		auto tokens = tokenize(line);
		auto rpn_tokens = shunt(tokens, operatordata_part1);
		resultsum_part1 += rpn(rpn_tokens);
	}
	cout << "Part 1: " << resultsum_part1 << endl;


	int64 resultsum_part2 = 0;
	for (auto& line : lines)
	{
		auto tokens = tokenize(line);
		auto rpn_tokens = shunt(tokens, operatordata_part2);
		resultsum_part2 += rpn(rpn_tokens);
	}
	cout << "Part 2: " << resultsum_part2 << endl;
}
