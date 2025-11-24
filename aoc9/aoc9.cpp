#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

auto get_input()
{
	std::string ln;
	std::getline(std::cin, ln);
	if(auto[m, pl, mar] = ctre::match<"(\\d+) players; last marble is worth (\\d+) points">(ln); m)
		return std::make_pair(pl.to_number<int>(), mar.to_number<int>());
	else
		fmt::println("parse fail {}", ln);

	return std::make_pair(0, 0);
}

int left_seven(int sz, int pos)
{
	if(pos >= 7)
		return pos - 7;
	return sz - 7 + pos;
}

int plus_one(int sz, int pos)
{
	pos += 2;
	if( pos < sz)
		return pos;
	return pos - sz;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto[pl, mar] = in;
	std::vector<int> circle{0, 16,  8, 17,  4, 18,  9, 19,  2, 20, 10, 21, 5, 11, 1, 12, 6, 13,  3, 14,  7, 15};
	std::vector<int> players(pl, 0);
	int c = 11;
	int p = 22 % pl;
	for(int m = 22; m <= mar; ++m, ++p)
	{
		if(p == pl)
			p = 0;
		if( m % 23 == 0)
		{
			players[p] += m;
			int l7 = left_seven(circle.size(), c);
			players[p] += circle[l7];
			circle.erase(circle.begin() + l7);
			c = l7;
		}
		else
		{
			c = plus_one(circle.size(), c);
			circle.insert(circle.begin() + c, m);
		}
	}
	return *std::ranges::max_element(players);
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	auto[pl, mar] = in;
	mar *= 100;
	std::vector<int> circle{0, 16,  8, 17,  4, 18,  9, 19,  2, 20, 10, 21, 5, 11, 1, 12, 6, 13,  3, 14,  7, 15};
	std::vector<int64_t> players(pl, 0);
	int c = 11;
	int p = 22 % pl;
	for(int m = 22; m <= mar; ++m, ++p)
	{
		if(p == pl)
			p = 0;
		if( m % 23 == 0)
		{
			players[p] += m;
			int l7 = left_seven(circle.size(), c);
			players[p] += circle[l7];
			circle.erase(circle.begin() + l7);
			c = l7;
		}
		else
		{
			c = plus_one(circle.size(), c);
			circle.insert(circle.begin() + c, m);
		}
	}
	return *std::ranges::max_element(players);
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
