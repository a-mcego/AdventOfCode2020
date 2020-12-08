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

struct Instruction
{
	string opcode;
	int parameter;

	explicit Instruction(string line)
	{
		auto parts = split(line," ");
		opcode = parts[0];
		parameter = FromString<int>(parts[1]);
	}
};

struct ProgramState
{
	int IP=0;
	int acc=0;
};

struct Program
{
	vector<Instruction> instructions;

	void AddInstruction(const Instruction& instruction)
	{
		instructions.push_back(instruction);
	}

	void RunStep(ProgramState& ps)
	{
		if (ps.IP >= (int)instructions.size())
		{
			cout << "IP out of bounds" << endl;
			return;
		}
		Instruction& ins = instructions[ps.IP];
		if (ins.opcode == "nop")
		{
			++ps.IP;
		}
		else if (ins.opcode == "acc")
		{
			ps.acc += ins.parameter;
			++ps.IP;
		}
		else if (ins.opcode == "jmp")
		{
			ps.IP += ins.parameter;
		}
		else
		{
			cout << "Illegal opcode " << ins.opcode << endl;
		}
	}
};

int main()
{
	auto lines = GetLines("8.txt");

	Program p;
	for(auto& line: lines)
		p.AddInstruction(Instruction(line));


	{
		ProgramState s;
		s.IP = 0; s.acc = 0;
		set<int> ip_positions;
		while (true)
		{
			if (ip_positions.find(s.IP) != ip_positions.end())
			{
				cout << "Part 1: accumulator is " << s.acc << endl;
				break;
			}
			ip_positions.insert(s.IP);
			p.RunStep(s);
		}
	}

	{
		//implementing Dijkstra for part 2
		struct Vertex { int distance=0x7FFFFFFF,previous_edge_id=-1; };
		struct Edge { int from=-1,to=-1; int weight=0; };

		vector<Vertex> vertices(p.instructions.size()+1, Vertex{});
		vector<Edge> edges;

		for (int id=0; id<p.instructions.size(); ++id)
		{
			auto& instr = p.instructions[id];
			if (instr.opcode == "acc")
				edges.push_back(Edge{ id,id + 1, 0 });
			else if (instr.opcode == "jmp")
			{
				edges.push_back(Edge{ id,id + instr.parameter, 0 });
				edges.push_back(Edge{ id,id + 1, 1 });
			}
			else if (instr.opcode == "nop")
			{
				edges.push_back(Edge{ id,id + 1, 0 });
				edges.push_back(Edge{ id,id + instr.parameter, 1 });
			}
		}

		map<int,vector<int>> edges_from_vertex;
		for (int i = 0; i < edges.size(); ++i)
			edges_from_vertex[edges[i].from].push_back(i);

		int start_vertex = 0, end_vertex = p.instructions.size(); //.size() because it's actually the one *right after* the last instruction
		vertices[start_vertex].distance = 0;

		set<int> remaining_vertices;
		for (int i = 0; i < vertices.size(); ++i)
			remaining_vertices.insert(i);

		while (!remaining_vertices.empty())
		{
			int min_vert_id = -1;
			for (auto& vert_id : remaining_vertices)
				if (min_vert_id == -1 || vertices[vert_id].distance < vertices[min_vert_id].distance)
					min_vert_id = vert_id;

			remaining_vertices.erase(min_vert_id);
			if (min_vert_id == end_vertex)
				break;

			for (auto edge_id : edges_from_vertex[min_vert_id])
			{
				Edge& edge = edges[edge_id];
				Vertex& vertex_to = vertices[edge.to];
				int alt = vertices[min_vert_id].distance + edge.weight;
				if (alt < vertex_to.distance)
				{
					vertex_to.distance = alt;
					vertex_to.previous_edge_id = edge_id;
				}
			}
		}

		int vert_child = end_vertex;
		vector<int> corrected_instructions;
		while (true)
		{
			if (vertices[vert_child].previous_edge_id == -1)
				break;
			Edge& edge = edges[vertices[vert_child].previous_edge_id];

			if (edge.weight == 1)
			{
				corrected_instructions.push_back(edge.from);
			}

			vert_child = edge.from;
		}

		if (corrected_instructions.size() != 1)
		{
			cout << "Part 2 ERROR: should have one corrected instruction but has " << corrected_instructions.size() << endl;
			return -1;
		}

		Program corrected_program = p;
		Instruction& instr = corrected_program.instructions[corrected_instructions[0]];

		if (instr.opcode == "jmp")
			instr.opcode = "nop";
		else if(instr.opcode == "nop")
			instr.opcode = "jmp";
		else
		{
			cout << "Part 2 ERROR: Instruction to be corrected is not 'jmp' or 'nop', is " << instr.opcode << endl;
			return -1;
		}

		ProgramState s;
		s.IP = 0; s.acc = 0;
		while (true)
		{
			if (s.IP == corrected_program.instructions.size())
				break;
			corrected_program.RunStep(s);
		}

		cout << "Part 2: accumulator is " << s.acc << endl;
	}
}
