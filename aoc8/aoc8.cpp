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

constexpr int w = 25;
constexpr int h = 6;
constexpr int wh = w * h;
using layer = std::array<char, 25 * 6>;

auto get_input()
{
	std::string ln;
	std::getline(std::cin, ln);
	return ln;
}

auto render(auto const& in)
{
	std::vector<layer> r;
	int n = 0;
	for(auto v: in)
	{
		if(n == 0)
			r.emplace_back();
		r.back()[n] = v - '0';
		++n;
		if(n == wh)
			n = 0;
	}
	return r;
}

auto combine(auto const& r)
{
	layer l;
	l.fill(8);
	for(auto& ll : r)
	{
		std::transform(l.begin(), l.end(), ll.begin(), l.begin(), [](auto le, auto re) -> char
		{
			switch(re)
			{
				case 0:
					return (le == 8) ? 0 : le;
				case 1:
					return (le == 8) ? 1: le;
				case 2:
					return le;			
			}
			return 0; // not reached
		});
	}

	return l;
}

void print(layer const& l)
{
	int p = 0;
	for(auto v = 0; v < h; ++v)
	{
		for(auto hr = 0; hr < w; ++hr)
		{
			fmt::print("{}", l[p] == 1 ? '*' : ' ');
			++p;
		}
		fmt::println("");
	}	
}

int64_t pt12(auto const& in)
{
	timer t("p2");
	auto r = render(in);
	auto o = combine(r);
	auto mn = std::ranges::min_element(r, [](auto& l, auto& r){ return std::ranges::count(l, 0) < std::ranges::count(r, 0);});
	fmt::println("p1 = {}", std::ranges::count(*mn, 1) * std::ranges::count(*mn, 2));
	fmt::println("p2 - ");
	print(o);
	return 0;
}

int main()
{
	auto in = get_input();
	pt12(in);
}
