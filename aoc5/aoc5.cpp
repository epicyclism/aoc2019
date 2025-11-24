#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "timer.h"

auto get_input()
{
	std::string ln;
	std::getline(std::cin, ln);
	return ln;
}

bool eq(char l, char r)
{
	return (l & 0x1f) == (r & 0x1f) && (l & 0xe0) != (r & 0xe0);
}

int64_t react(std::string s)
{
	auto it = s.begin();
	while((it = std::ranges::adjacent_find(s, eq)) != s.end())
		s.erase(it, it + 2);
	return s.size();
}

int64_t react2(std::string s, char c)
{
	std::erase_if(s, [=](auto c1){ return !((c ^ c1) & 0x1f);});
	auto it = s.begin();
	while((it = std::ranges::adjacent_find(s, eq)) != s.end())
		s.erase(it, it + 2);
	return s.size();
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	return react(in);
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	int64_t mn = in.size();
	for(char rc = 'a'; rc <= 'z'; ++rc)
	{
		auto m = react2(in, rc);
		if( m < mn)
			mn = m;
	}
	return mn;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
