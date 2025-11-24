#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct pv
{
	int px_;
	int py_;
	int vx_;
	int vy_;
	void step()
	{
		px_ += vx_;
		py_ += vy_;
	}
};

auto get_input()
{
	std::vector<pv> r;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, px, py, vx, vy] = ctre::match<"position=<([ |\\-]?\\d+), ([ |\\-]?\\d+)> velocity=<([ |\\-]?\\d+), ([ |\\-]?\\d+)>">(ln); m)
			r.emplace_back(svp_to_t<int>(px), svp_to_t<int>(py), svp_to_t<int>(vx), svp_to_t<int>(vy));
		else
			fmt::println("parse fail {}", ln);
	}
	return r;
}

bool neighbours(auto const& a, auto const& b)
{
	int dx = std::abs(a.px_ - b.px_);
	int dy = std::abs(a.py_ - b.py_);
	if( dx == 0 && dy == 0)
		return false;
	return dx < 2 && dy < 2;
}

bool finished(auto const& in)
{
	for(auto const& p: in)
	{
		for(auto const& p2: in)
			if(neighbours(p, p2))
				goto next;
		return false;
next:
	}
	return true;
}

void render(auto const& in)
{
	auto ox = std::ranges::min_element(in, std::less{}, &pv::px_)->px_;
	auto oy = std::ranges::min_element(in, std::less{}, &pv::py_)->py_;
	auto wx = std::ranges::max_element(in, std::less{}, &pv::px_)->px_ - ox + 1;
	auto wy = std::ranges::max_element(in, std::less{}, &pv::py_)->py_ - oy + 1;
	std::vector<std::string> raster;
	for(int n = 0; n < wy; ++n)
		raster.emplace_back(std::string(wx, '.'));
	for(auto const& p: in)
		raster[p.py_ - oy][p.px_ - ox] = '#';
	int n = 0;
	for(auto const& r: raster)
		fmt::println("{}", r);
}

int pt12(auto in)
{
	timer t("p12");
	int tm = 0;
	while(!finished(in))
	{
		for(auto& l : in)
			l.step();
		++tm;
	}
	render(in);
	return tm;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	return 0;
}

int main()
{
	auto in = get_input();
	auto p2 = pt12(in);
	fmt::println("pt2 = {}", p2);
}
