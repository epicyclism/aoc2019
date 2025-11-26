#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

auto get_input()
{
	std::vector<int> v;
	std::string ln;
	while(std::getline(std::cin, ln))
		v.emplace_back(sv_to_t<int>(ln));
	return v;
}

int pt1(auto const& in)
{
	timer t("p1");

	return std::ranges::fold_left(in | std::ranges::views::transform([](auto v){ return v / 3 - 2;}), 0, std::plus<>());
}

int pt2(auto const& in)
{
	timer t("p2");
	auto ff = [](auto f)
		{
			auto sm = 0;
			auto fi = f;
			while(1)
			{
				fi = fi / 3 - 2;
				if(fi <= 0)
					break;
				sm += fi;
			}
			return sm;
		};
	return std::ranges::fold_left(in | std::ranges::views::transform(ff), 0, std::plus<>());
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1  = {}", p1);
	fmt::println("pt2  = {}", p2);
}
