#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include "intcode.h"

#include "ctre_inc.h"
#include "timer.h"

int pt1(auto const& in)
{
	timer t("p1");
	std::array<int, 5> phases = {0, 1, 2, 3, 4};
	int p1 = 0;
	do
	{
		int64_t inout = 0;
		for(auto p: phases)
		{
			auto wkr(in);
			wkr.in_.push(p);
			wkr.in_.push(inout);
			wkr.execute();
			inout = wkr.buf_;
		}
		if(inout > p1)
			p1 = inout;
	} while (std::ranges::next_permutation(phases).found);
	
	return p1;
}

int pt2(auto const& in)
{
	timer t("p2");
	std::array<int, 5> phases = {5, 6, 7, 8, 9};
	int p2 = 0;
	do
	{
		std::array         procs = {in, in, in, in, in};
		int64_t inout = 0;
		for(int n = 0; n < 5; ++n)
			procs[n].in_.push(phases[n]);
		while(!procs[0].halted_)
		{
			for(auto& p : procs)
			{
				p.in_.push(inout);
				p.execute();
				inout = p.buf_;
			}
		}
		if(inout > p2)
			p2 = inout;
	} while (std::ranges::next_permutation(phases).found);
	
	return p2;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
