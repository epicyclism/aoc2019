#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <variant>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

struct record_t
{
	std::string				date_;
	int         			tm_;
	std::variant<int, bool> dat_; // guard no or awake, asleep

	record_t(std::string_view v, int t, int g) : date_(v), tm_(t), dat_(g)
	{}
	record_t(std::string_view v, int t, bool b) : date_(v), tm_(t), dat_(b)
	{}
};

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

auto get_input()
{
	std::vector<record_t> v;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, d, th, tm, g] = ctre::match<"\\[([\\d\\-]+) (\\d+):(\\d+)\\] Guard .(\\d+) begins shift">(ln); m)
			v.emplace_back(d, th.to_number<int>() * 60 + tm.to_number<int>(), g.to_number<int>());
		else
		if(auto[m, d, th, tm] = ctre::match<"\\[([\\d\\-]+) (\\d+):(\\d+)\\] wakes up">(ln); m)
			v.emplace_back(d, th.to_number<int>() * 60 + tm.to_number<int>(), true);
		else
		if(auto[m, d, th, tm] = ctre::match<"\\[([\\d\\-]+) (\\d+):(\\d+)\\] falls asleep">(ln); m)
			v.emplace_back(d, th.to_number<int>() * 60 + tm.to_number<int>(), false);
		else
			fmt::println("parse fail {}", ln);
	}
	std::ranges::sort(v, [](auto& l, auto& r){if (l.date_ == r.date_) return l.tm_ < r.tm_; return l.date_ < r.date_;});
	return v;
}

struct guard_time_t
{
	int total_;
	std::array<int, 60> over_;
	guard_time_t() : total_(0)
	{
		over_.fill(0);
	}
};

auto pt12(auto const& in)
{
	timer t("p12");
	std::map<int, guard_time_t> mp;
	int cur_g = -1;
	int begin = -1;
	for(auto& r: in)
	{
		std::visit(overloaded{[&](int arg){ cur_g = arg; begin = -1;},
								[&](bool aw)
								{
									if(!aw)
										begin = r.tm_ % 60;
									else
										for(int t = begin; t < r.tm_ % 60; ++t)
										{
											++mp[cur_g].total_;
											++mp[cur_g].over_[t];
										}
								}},r.dat_);
	}
	auto mx = std::ranges::max_element(mp, [](auto& l, auto& r){ return l.second.total_ < r.second.total_;});
	auto mxh = std::ranges::max_element((*mx).second.over_);
	int p1 = (*mx).first * std::distance((*mx).second.over_.begin(), mxh);
	int mx2 = 0;
	int p2 = 0;
	for(auto& gr: mp)
	{
		auto mxt = std::ranges::max_element(gr.second.over_);
		if( *mxt > mx2)
		{
			mx2 = *mxt;
			p2 = gr.first * std::distance(gr.second.over_.begin(), mxt);
		}
	}
	return std::make_pair(p1, p2);
}

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
