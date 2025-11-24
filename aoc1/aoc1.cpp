#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <algorithm>
#include <numeric>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

auto get_input()
{
	std::vector<int> v;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, n] = ctre::match<"\\+?(-?\\d+)">(ln); m)
			v.push_back(n.to_number<int>());
	}
	return v;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	return std::reduce(std::begin(in), std::end(in), 0);
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	std::bitset<512688> bs;
	int f = 0;
	while(1)
	{
		for(auto d : in)
		{
			f += d;
			if(bs.test(f + 256344))
				goto bail;
			bs.set(f + 256344);
		}
	}
bail:
	return f;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
