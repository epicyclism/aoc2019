#include <iostream>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "graph.h"
#include "timer.h"

int encode(std::string_view s)
{
	if(s.length() > 3)
	{
		fmt::println("bogus planet {}", s);
		return 0;
	}
	int rv = 0;
	for(auto c: s)
	{
		rv <<= 6;
		rv |= (c - '0') & 0x3f;
	}
	return rv;
}

std::string decode(int n)
{
	std::string s;
	while(n)
	{
		s.push_back((n & 0x3f) + '0');
		n >>= 6;
	}
	return s;
}

using graph_t = std::vector<std::vector<int>>;

void add_edge(graph_t& g, std::string_view fs, std::string_view ts)
{
	auto f = encode(fs);
	auto t = encode(ts);
	if(g.size() <= f)
		g.resize( f + 1);
	g[f].emplace_back(t);
}

void dump(graph_t const& g)
{
	int n = 0;
	for(auto& al : g)
	{
		if(al.size())
		{
			fmt::print("{} : ", decode(n));
			for(auto e : al)
				fmt::print("{} ", decode(e));
			fmt::println("");
		}
		++n;
	}
}

auto get_input()
{
	graph_t g1;
	graph_t g2;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, fs, ts] = ctre::match<"(\\w+)\\)(\\w+)">(ln); m)
		{
			add_edge(g1, fs, ts);
			add_edge(g2, fs, ts);
			add_edge(g2, ts, fs);
		}
		else
			fmt::println("parse fail {}", ln);
	}
	return std::make_pair(g1, g2);
}

void count_from( graph_t const& g, int v, int& r)
{
	for(auto t : g[v])
	{
		++r;
		count_from(g, t, r);
	}
}

int pt1(auto const& in)
{
	timer t("p1");
	int p1 = 0;
	for(int n = 0; n < in.size(); ++n)
		if(in[n].size())
		count_from(in, n, p1);
	return p1;
}

int pt2(auto const& in)
{
	timer t("p2");
	auto d = bfs(in, encode("YOU"));
	return d[encode("SAN")] - 2;
}

int main()
{
	auto [in1, in2] = get_input();
	auto p1 = pt1(in1);
	auto p2 = pt2(in2);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
