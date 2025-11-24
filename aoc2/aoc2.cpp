#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "timer.h"

auto get_input()
{
	std::vector<std::string> v;
	std::string ln;
	while(std::getline(std::cin, ln))
		v.emplace_back(std::move(ln));
	return v;
}

int pt1(auto const& in)
{
	timer t("p1");
	std::array<int, 26> cnts;
	int twos = 0;
	int threes = 0;
	for(auto& i : in)
	{
		cnts.fill(0);
		for(auto c: i)
			++cnts[c - 'a'];
		if(std::ranges::find(cnts, 2) != cnts.end())
			++twos;
		if(std::ranges::find(cnts, 3) != cnts.end())
			++threes;
	}
	return twos * threes;
}

bool diff_1(std::string const& a, std::string const& b)
{
	bool seen_one = false;
	// assume equal len...
	for(size_t s = 0; s < a.size(); ++s)
		if( a[s] != b[s])
		{
			if(seen_one)
				return false;
			seen_one = true;
		}
	return seen_one;
}

std::string common(std::string const& a, std::string const& b)
{
	std::string r;
	for(size_t s = 0; s < a.size(); ++s)
		if( a[s] == b[s])
			r.push_back(a[s]);
	return r;
}

std::string pt2(auto const& in)
{
	timer t("p2");
	for(size_t a = 0; a < in.size() - 1; ++a)
		for(size_t b = 1; b < in.size(); ++b)
		{
			if(diff_1(in[a], in[b]))
				return common(in[a], in[b]);
		}
	return "";
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
