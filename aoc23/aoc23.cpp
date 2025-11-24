#include <iostream>
#include <string>
#include <vector>

#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct bot
{
	int64_t x_;
	int64_t y_;
	int64_t z_;
	int64_t r_;
};

inline int64_t md(bot const& l, bot const& r)
{
	return std::abs(l.x_ - r.x_) +
			std::abs(l.y_ - r.y_) +
			std::abs(l.z_ - r.z_);
}

inline int64_t md(bot const& l)
{
	return std::abs(l.x_) +
			std::abs(l.y_) +
			std::abs(l.z_);
}

auto get_input()
{
	std::vector<bot> vb;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, x, y, z, r] = ctre::match<"pos=<(-?\\d+),(-?\\d+),(-?\\d+)>, r=(-?\\d+)">(ln); m)
			vb.emplace_back(x.to_number<int64_t>(), y.to_number<int64_t>(), z.to_number<int64_t>(), r.to_number<int64_t>());
		else
			fmt::println("parse fail {}", ln);
	}
	return vb;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto br = std::ranges::max_element(in, {}, &bot::r_);

	return std::ranges::count_if(in, [&](auto const& b){ return md(*br, b) <= (*br).r_;});
}

// calculate the min and max reachable distances from 0 for each bot.
// look for the most overlapping bots.
// problem I think designed to fit this solution.
// inspired by mega thread.
//
int64_t pt2(auto const& in)
{
	timer t("p2");
	std::vector<std::pair<int64_t, int64_t>> v(in.size());
	for(auto& b: in)
	{
		auto d = md(b);
		v.emplace_back(std::max(0LL, d - b.r_), 1);
		v.emplace_back(d + b.r_ + 1, -1 );
	}
	std::ranges::sort(v);
	int64_t cnt = 0;
	int64_t mx  = 0;
	int64_t D   = 0;
	for(auto& t: v)
	{
		cnt += t.second;
		if(cnt > mx)
		{
			mx = cnt;
			D = t.first;
		}
	}
	return D;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
