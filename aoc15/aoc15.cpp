#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "graph.h"
#include "timer.h"

auto get_input()
{
	std::vector<char> v;
	size_t stride = 0;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(stride == 0)
			stride = ln.size();
		v.insert(v.end(), ln.begin(), ln.end());
	}
	return std::make_pair(stride, v);
}

void dump(size_t stride, std::vector<char> const& g)
{
	size_t p = 0;
	while(p < g.size())
	{
		fmt::println("{}", fmt::join(g.begin() + p, g.begin() + p + stride, ""));
		p += stride;
	}
}

struct unit_t
{
	vertex_id_t	pos_;
	int			attack_power_;
	int			hit_points_;
	char		type_;
};

void dump(std::vector<unit_t> const& units)
{
	for(auto& u: units)
		if(u.hit_points_ > 0)
			fmt::println("{} {} {} {}", u.type_, u.pos_, u.attack_power_, u.hit_points_ );
}

auto get_units(std::vector<char> const& v, int hp)
{
	std::vector<unit_t> rv;
	for(auto p = 0; p < v.size(); ++p)
		if( v[p] == 'E')
			rv.emplace_back(p, hp, 200, v[p]);
		else
		if(v[p] == 'G')
			rv.emplace_back(p, 3, 200, v[p]);
	return rv;
}

size_t unit_from_pos(std::vector<unit_t> const& units, vertex_id_t p)
{
	for(size_t n = 0; n < units.size(); ++n)
		if(units[n].pos_ == p && units[n].hit_points_ > 0)
			return n;
	return -1;
}

bool press_attack(unit_t const& u, std::vector<unit_t>& units, size_t stride, std::vector<char>& grid )
{
	std::vector<vertex_id_t> adj;
	if(u.type_ == 'G')
	{
		grid_direct_border_ro grid_adj_e(grid, stride, [&](auto p){ return grid[p] == 'E';});
		adj = grid_adj_e[u.pos_];
	}
	else
	{
		grid_direct_border_ro grid_adj_g(grid, stride, [&](auto p){ return grid[p] == 'G';});
		adj = grid_adj_g[u.pos_];
	}
	if(adj.empty())
		return false;
	std::ranges::stable_sort(adj, [&](auto l, auto r){ return units[unit_from_pos(units, l)].hit_points_ < units[unit_from_pos(units, r)].hit_points_;});
	auto enemy_pos = unit_from_pos(units, adj[0]);
	units[enemy_pos].hit_points_ -= u.attack_power_;
	if(units[enemy_pos].hit_points_ <= 0)
		grid[adj[0]] = '.';
	return true;
}

std::vector<vertex_id_t> adjacent_set(char type, std::vector<unit_t> const& units, size_t stride, std::vector<char> const& grid)
{
	std::vector<vertex_id_t> rv;
	grid_direct_border_ro grid_adj(grid, stride, [&](auto p){ return grid[p] == '.';});
	for(auto& u: units)
	{
		if(u.type_ == type && u.hit_points_ > 0)
		{
			auto a = grid_adj[u.pos_];
			rv.insert(rv.end(), a.begin(), a.end());
		}
	}
	return rv;
}

void move(unit_t& u, std::vector<unit_t>& units, size_t stride, std::vector<char>& grid )
{
	grid_direct_border_ro grid_move (grid, stride, [&](auto p){ return grid[p] == '.';});
	auto tgts = adjacent_set(u.type_ == 'G' ? 'E' : 'G', units, stride, grid);
	if(tgts.empty())
		return;
	// now find the shortest paths. 
	auto[d, p] = bfs<decltype(grid_move), true>(grid_move, u.pos_);
	std::erase_if(tgts, [&](auto l){ return d[l] == -1;});
	if(tgts.empty())
		return;
	std::ranges::sort(tgts, [&](auto l, auto r)
	{
		if(d[l] == d[r])
			return l < r;
		return d[l] < d[r]; 
	});
	// do the move
	if(tgts[0] == -1) // can't happen?
		return;
	// trace the path back from tgts[0] and take the first step
	auto x = tgts[0];
	while(p[x] != u.pos_)
		x = p[x];
	grid[u.pos_] = '.';
	grid[x] = u.type_;
	u.pos_ = x;
}

bool no_opponents(char type, std::vector<unit_t> const& units)
{
	if(type == 'E')
		return std::ranges::count_if(units, [](auto& u2){ return u2.type_ == 'G' && u2.hit_points_ > 0;} ) == 0;
	return std::ranges::count_if(units, [](auto& u2){ return u2.type_ == 'E'&& u2.hit_points_ > 0;} ) == 0;
}

auto proc(size_t stride, std::vector<char> grid, int hp)
{
	auto units = get_units(grid, hp);
	int rnd = 0;
	while(1)
	{
		for(auto& u: units)
		{
			if(u.hit_points_ <= 0)
				continue;
			// any opponents?
			if(no_opponents(u.type_, units))
				goto bail;
			// can attack immediately?
			if(!press_attack(u, units, stride, grid))
			{
				// if not move
				move(u, units, stride, grid);
				// can attack now?
				press_attack(u, units, stride, grid);
			}
		}
		std::ranges::sort(units, {}, &unit_t::pos_);
		++rnd;
	}
bail:
#if 0
	fmt::println("rnd {}", rnd);
	dump(stride, grid);
	dump(units);
#endif
	return std::make_pair(rnd, units);
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto[stride, grid] = in;
	auto[rnd, units] = proc(stride, grid, 3);
	return rnd * std::ranges::fold_left(units, 0, [](auto l, auto& r){ return l + (r.hit_points_ > 0 ? r.hit_points_ : 0);});
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	auto[stride, grid] = in;
	auto elf_count = std::ranges::count(grid, 'E');
	int hp = 4;
	while(1)
	{
		auto[rnd, units] = proc(stride, grid, hp);
		if(std::ranges::count_if(units, [](auto& u){ return u.type_ == 'E' && u.hit_points_ > 0;}) == elf_count)
			return rnd * std::ranges::fold_left(units, 0, [](auto l, auto& r){ return l + (r.hit_points_ > 0 ? r.hit_points_ : 0);});
		++hp;
	}
	return 0;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
