#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

struct intcode_wsp;
void plus(intcode_wsp& ic);
void times(intcode_wsp& ic);
void invalid(intcode_wsp& ic);
void end(intcode_wsp&);

struct intcode_wsp
{
	size_t pc_ = 0;
	std::vector<int64_t> mem_;
	void (*acts_[100])(intcode_wsp&) =
	{ 	invalid,    plus,   times, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid,
		invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, invalid, end
	};
	void execute()
	{
		acts_[mem_[pc_]](*this);
	}
};

void plus(intcode_wsp& ic)
{
	ic.mem_[ic.mem_[ic.pc_ + 3]] = ic.mem_[ic.mem_[ic.pc_ + 1]] + ic.mem_[ic.mem_[ic.pc_ + 2]];
//	fmt::println("+ {}", ic.wsp_);
	ic.pc_ += 4;
	ic.acts_[ic.mem_[ic.pc_]](ic);
}

void times(intcode_wsp& ic)
{
	ic.mem_[ic.mem_[ic.pc_ + 3]] = ic.mem_[ic.mem_[ic.pc_ + 1]] * ic.mem_[ic.mem_[ic.pc_ + 2]];
//	fmt::println("* {}", ic.wsp_);
	ic.pc_ += 4;
	ic.acts_[ic.mem_[ic.pc_]](ic);
}

void invalid(intcode_wsp& ic)
{
	fmt::println("{}", ic.mem_);
	fmt::println("bogus instruction");
	return;
}

void end(intcode_wsp& ic)
{
	return;
}

auto get_input()
{
	intcode_wsp ic;
	std::string ln;
	std::getline(std::cin, ln);
	for(auto i: ctre::search_all<"(-?\\d+)">(ln))
		ic.mem_.emplace_back(i.to_number<int64_t>());
	return ic;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto wkr(in);
	wkr.mem_[1] = 12;
	wkr.mem_[2] = 02;
	wkr.execute();
	return wkr.mem_[0];
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	for(int noun = 0; noun < 100; ++noun)
		for(int verb = 0; verb < 100; ++verb)
		{
			auto wkr (in);
			wkr.mem_[1] = noun;
			wkr.mem_[2] = verb;
			wkr.execute();
			if(wkr.mem_[0] == 19690720)
				return 100 * noun + verb;
		}
	return 0;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
