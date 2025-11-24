#include <iostream>
#include <string>
//#include <algorithm>
//#include <numeric>
//#include <ranges>
#include <vector>
#include <queue>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct pt_t
{
	int8_t a_;
	int8_t b_;
	int8_t c_;
	int8_t d_;
};

int md(pt_t const& a, pt_t const& b)
{
	return std::abs(a.a_ - b.a_) + std::abs(a.b_ - b.b_) +
			std::abs(a.c_ - b.c_) + std::abs(a.d_ - b.d_);
}

auto get_input()
{
	std::vector<pt_t> v;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, a, b, c, d] = ctre::match<"(-?\\d),(-?\\d),(-?\\d),(-?\\d)">(ln); m)
			v.emplace_back(a.to_number<int8_t>(), b.to_number<int8_t>(), c.to_number<int8_t>(), d.to_number<int8_t>());
		else
			fmt::println("parse fail {}", ln);
	}
	return v;
}

auto build_graph(auto const& p)
{
	std::vector<std::vector<int>> g(p.size());
	for(auto f = 0; f < p.size() - 1; ++f)
		for(auto t = f + 1; t < p.size(); ++t)
			if(md(p[f], p[t]) < 4)
			{
				g[f].push_back(t);
				g[t].push_back(f);
			}

	return g;
}

void bfs(auto const& g, int id_from, std::vector<bool>& visited) 
{
    std::queue<int> q;
    q.push(id_from); 
    visited[id_from] = true;
    while (!q.empty())
    {
        auto u = q.front(); q.pop();
        for (auto e : g[u])
        {
            if (!visited[e])
            {
                visited[e] = true;
                q.push(e);
            }
        }
    }
}  

int64_t pt1(auto in)
{
	timer t("p1");
	auto g = build_graph(in);
	int cons = 0;
	std::vector<bool> visited(g.size());
	for(auto f = 0; f < g.size(); ++f)
	{
		if(!visited[f])
		{
			++cons;
			bfs(g, f, visited);
		}
	}
	return cons;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	fmt::println("pt1 = {}", p1);
}
