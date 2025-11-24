#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct claim_t
{
	int x_;
	int y_;
	int cx_;
	int cy_;
};

auto get_input()
{
	std::vector<claim_t> v;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, x, y, cx, cy] = ctre::search<"(\\d+),(\\d+): (\\d+)x(\\d+)">(ln); m)
			v.emplace_back(x.to_number<int>(), y.to_number<int>(), cx.to_number<int>(), cy.to_number<int>());
		else
			fmt::println("parse fail {}", ln);
	}
	return v;
}

template<size_t S> void fill(std::vector<char>& bs, int x, int y, int cx, int cy)
{
	for(auto line = 0; line < cy; ++line)
	{
		size_t pt = (line + y) * S + x;
		for(auto dx = 0; dx < cx; ++dx)
			++bs[pt + dx];
	}
}

template<size_t S> bool test(std::vector<char>& bs, int x, int y, int cx, int cy)
{
	for(auto line = 0; line < cy; ++line)
	{
		size_t pt = (line + y) * S + x;
		for(auto dx = 0; dx < cx; ++dx)
			if(bs[pt + dx] == 0 || bs[pt + dx] > 1)
				return false;
	}
	return true;
}

auto pt12(auto const& in)
{
	timer t("p12");
	std::vector<char> v(1024*1024);
	for(auto& c: in)
		fill<1024>(v, c.x_, c.y_, c.cx_, c.cy_);
	size_t p2 = 0;
	for(; p2 < in.size(); ++p2)
		if(test<1024>(v, in[p2].x_, in[p2].y_, in[p2].cx_, in[p2].cy_))
			break;
	return std::make_pair(std::ranges::count_if(v, [](auto c){ return c > 1;}), p2 + 1); // claims start at 1
}

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
