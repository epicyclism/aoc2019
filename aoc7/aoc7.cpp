#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct task
{
	bool active_;
	int  time_;
	std::array<bool, 26> depends_;
	task()
	{
		active_ = false;
		time_ = 0;
		depends_.fill(false);
	}
};

using graph_t = std::array<task, 26>;

auto get_input()
{
	graph_t g;

	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, t, f] = ctre::match<"Step ([A-Z]) must be finished before step ([A-Z]) can begin.">(ln); m)
		{
			int tc = t.view()[0] - 'A';
			int fc = f.view()[0] - 'A';
			g[tc].active_ = true;
			g[fc].active_ = true;
			g[fc].depends_[tc] = true;
		}
		else
			fmt::println("parse error {}", ln);
	}
	return g;
}

std::string topo_sort(auto in)
{
	std::string s;

	while(std::ranges::any_of(in, [](auto& e){ return e.active_;}))
	{
		for(int c = 0; c < in.size(); ++c)
		{
			auto& a = in[c];
			if(a.active_ && std::ranges::none_of(a.depends_, std::identity()))
			{
				s.push_back(c + 'A');
				for(auto& d : in)
					d.depends_[c] = false;
				a.active_ = false;
				break;
			}
		}
	}
	return s;
}

std::string pt1(auto const& in)
{
	timer t("p1");
	return topo_sort(in);
}

int topo_sort2(auto in)
{
	int tm = 0;
	int wkrs = 0;
	auto nt = std::ranges::count_if(in, [](auto& t){return t.active_;});
	const int mx_wkrs = nt > 10 ? 5 : 2;
	const int base = nt > 10 ? 60 : 0;
	for(int c = 0; c < in.size(); ++c)
		if(in[c].active_)
		{
			in[c].active_ = false;
			in[c].time_ = base + c + 1;
		}
	while(std::ranges::any_of(in, [](auto& e){ return e.time_ > 0;}))
	{
		for(int c = 0; c < in.size(); ++c)
		{
			auto& a = in[c];
			if(wkrs < mx_wkrs && a.time_ && !a.active_ && std::ranges::none_of(a.depends_, std::identity()))
			{
				++wkrs;
				a.active_ = true;
			}
		}
		for(int c = 0; c < in.size(); ++c)
		{
			auto& a = in[c];
			if(a.active_)
			{
				--a.time_;
				if(a.time_ == 0)
				{
					for(auto& d : in)
						d.depends_[c] = false;
					--wkrs;
					a.active_ = false;
				}
			}
		}
		++tm;
	}
	return tm;
}

int pt2(auto const& in)
{
	timer t("p2");
	return topo_sort2(in);
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
