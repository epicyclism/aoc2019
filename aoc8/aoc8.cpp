#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

using graph_t = std::vector<std::vector<int>>;

int const* read_state(graph_t& g, graph_t& m, int n, int const* p)
{
	int ns = *p++;
	int nm = *p++;
	int id = g.size();
	g.emplace_back();
	m.emplace_back();
	for(int s = 0; s < ns; ++s)
	{
		g[n].emplace_back(g.size());
		p = read_state(g, m, g.size(), p);
	}
	for(int mm = 0; mm < nm; ++mm)
		m[id].emplace_back(*p++);
	
	return p;
}

std::pair<graph_t, graph_t> load(std::vector<int> const& v)
{
	graph_t g;
	graph_t m;
	read_state(g, m, 0, v.data());

	return std::make_pair(g, m);
}

auto get_input()
{
	std::vector<int> v;
	std::string ln;
	while(std::getline(std::cin, ln))
		for(auto i : ctre::search_all<"(\\d+)">(ln))
			v.emplace_back(i.to_number<int>());

	return v;
}

int value(graph_t const& g, graph_t const& m, int n)
{
	if(n >= g.size())
		return 0;
	if(g[n].size() == 0)
		return std::ranges::fold_left(m[n], 0, std::plus());
	int sm = 0;
	for(auto nn : m[n])
		if(nn <= g[n].size() && nn > 0)
			sm += value(g, m, g[n][nn - 1]);

	return sm;
}

auto pt12(auto const& in)
{
	timer t("p1");
	auto[g, m] = load(in);
	auto p1 = std::ranges::fold_left(m, 0, [](auto l, auto& r){return std::ranges::fold_left(r, l, std::plus());});
	auto p2 = value(g, m, 0);
	return std::make_pair(p1, p2);
}

int main()
{
	auto in = get_input();
	auto[ p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
