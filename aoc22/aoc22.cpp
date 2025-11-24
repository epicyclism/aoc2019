#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <queue>

#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

auto get_input()
{
	int depth = 0;
	int tgtx = 0;
	int tgty = 0;
	std::string ln;
	std::getline(std::cin, ln);
	if(auto[m, d] = ctre::match<"depth: (\\d+)">(ln); m)
		depth = d.to_number<int>();
	std::getline(std::cin, ln);
	if(auto[m, x, y] = ctre::match<"target: (\\d+),(\\d+)">(ln); m)
	{
		tgtx = x.to_number<int>();
		tgty = y.to_number<int>();
	}
	return std::make_tuple(depth, tgtx, tgty);
}

constexpr int neither {0};
constexpr int climbing {1};
constexpr int torch {2};

constexpr int rocky {0};
constexpr int wet {1};
constexpr int narrow {2};

constexpr int border = 50;

using vertex_t = int;
using edge_t  = struct{ int wt_; int to_;};
using graph_t = std::vector<std::vector<edge_t>>;

void add_vertex(vertex_t id, graph_t& g)
{
    while( g.size() <= id)
        g.push_back({});
}

void add_edge(vertex_t from, vertex_t to, int weight, graph_t& g)
{
    add_vertex(from, g);
    add_vertex(to, g);
    g[from].push_back({weight, to});
}

bool cross_region( int type, int tool)
{
	if(tool >= 3)
		fmt::println("bogus tool in cross region {}", tool);
    switch(type)
    {
        case rocky:
            return tool != neither;
        case wet:
            return tool != torch ;
        case narrow:
            return tool != climbing;
        default:
			fmt::println("bogus cross_region {} {}", type, tool);
            return false;
    }
}

struct cave_explorer
{
	int depth_;
	int tgtx_;
	int tgty_;
	int stride_;
	std::vector<int64_t> cache_;
	cave_explorer(int d, int x, int y) : depth_(d), tgtx_(x), tgty_(y), stride_(tgtx_ + border), cache_(stride_ * (tgty_ + border), -1)
	{}
	int erosion_level(int x, int y)
	{
		int hp = y * stride_ + x;
		if(cache_[hp] != -1)
			return cache_[hp];
		int gi = 0;
		if((x == 0 && y == 0) || (x == tgtx_ && y == tgty_))
			gi = 0;
		else
		if(x == 0)
			gi = y * 48271;
		else
		if(y == 0)
			gi = x * 16807;
		else
			gi = erosion_level(x-1, y) * erosion_level(x, y - 1);
		auto el = (gi + depth_) % 20183;
		cache_[hp] = el;

		return el;
	}
	int type(int x, int y)
	{
		return erosion_level(x, y) % 3;
	}
	vertex_t vertex_id_from_region_tool(int x, int y, int tool)
	{
		if(tool >= 3)
			fmt::println("bogus tool {} at vertex {} {}", tool, x, y);
    	return (y * stride_ + x) * 3 + tool;
	}
	// a region has three vertices, representing each 'tool',
	// and edges to represent changing between each (7 min).
	// the edge out of each vertex represents crossing this region
	// and entering theh next (1min).
	//
	void install_region(int x, int y, graph_t& g)
	{
		auto v_neither  = vertex_id_from_region_tool(x, y, neither);
		auto v_climbing = vertex_id_from_region_tool(x, y, climbing);
		auto v_torch    = vertex_id_from_region_tool(x, y, torch);
		// vertex for each tool
		add_vertex(v_neither, g);
		add_vertex(v_climbing, g);
		add_vertex(v_torch, g);    
		// tool swaps
		add_edge(v_neither, v_climbing, 7, g);
		add_edge(v_neither, v_torch, 7, g);
		add_edge(v_climbing, v_neither, 7, g);
		add_edge(v_climbing, v_torch, 7, g);
		add_edge(v_torch, v_neither, 7, g);
		add_edge(v_torch, v_climbing, 7, g);
	}
	graph_t build_graph ()
	{
		graph_t g;
		// install the regions
		for( int y = 0; y < tgty_ + border; ++y)
			for(int x = 0; x < stride_; ++x)
				install_region(x, y, g);
		// connect them together
		for( int y = 0; y < tgty_ + border - 1; ++y)
			for(int x = 0; x < stride_ - 1; ++x)
			{
				for( int tool = 0; tool < 3; ++tool)
				{
					int tp = type(x, y);
					if( cross_region(tp, tool))
					{
						if( x > 0) // left
							add_edge(vertex_id_from_region_tool(x, y, tool), vertex_id_from_region_tool(x - 1, y, tool), 1, g);
						// right
						add_edge(vertex_id_from_region_tool(x, y, tool), vertex_id_from_region_tool(x + 1, y, tool), 1, g);
						if ( y > 0) // up
							add_edge(vertex_id_from_region_tool(x, y, tool), vertex_id_from_region_tool(x, y - 1, tool), 1, g);
						// down
						add_edge(vertex_id_from_region_tool(x, y, tool), vertex_id_from_region_tool(x, y + 1, tool), 1, g);
					}
				}
			}
		return g;
	}
};

int pt1(auto const& in)
{
	timer t("p1");
	cave_explorer exp(std::get<0>(in), std::get<1>(in), std::get<2>(in));
	int p1 = 0;
	for(int x = 0; x <= exp.tgtx_; ++x)
		for(int y = 0; y <= exp.tgty_; ++y)
			p1 += exp.type(x, y);

	return p1;
}

std::vector<int> dijkstra(int from, graph_t const& g)
{
	std::vector<int> d(g.size(), std::numeric_limits<int>::max());
    auto pq_t_cmp = [&](auto& l, auto& r){ return d[l] > d[r];};
    std::priority_queue<int, std::vector<int>, decltype(pq_t_cmp)> q(pq_t_cmp);
    q.push(from);
	d[from] = 0;
    while (!q.empty())
    {
        auto p = q.top(); q.pop();
        for( auto e : g[p])  
        {
            if (d[e.to_] > d[p] + e.wt_)
            {
                d[e.to_] = d[p] + e.wt_;
                q.push(e.to_);
            }
        }
    }

	return d;
}

std::pair<int, int> pt12(auto const& in)
{
	timer t("p12");
	cave_explorer exp(std::get<0>(in), std::get<1>(in), std::get<2>(in));
	int p1 = 0;
	for(int x = 0; x <= exp.tgtx_; ++x)
		for(int y = 0; y <= exp.tgty_; ++y)
			p1 += exp.type(x, y);
			
	auto g = exp.build_graph();
	auto d = dijkstra(exp.vertex_id_from_region_tool(0, 0, torch), g);
	return {p1, d[exp.vertex_id_from_region_tool(exp.tgtx_, exp.tgty_, torch)]};
}

// 1069 too low!
// 1071 too low!
// 1082 too high!
// 1079 wrong

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
//	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
