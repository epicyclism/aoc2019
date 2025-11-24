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

struct recipe_list
{
	std::vector<uint8_t> wsp_;
	size_t A_;
	size_t B_;
	recipe_list (int sz = 32)
	{
		wsp_.reserve(sz);
		A_ = 0;
		B_ = 1;
		wsp_.push_back(3);
		wsp_.push_back(7);
	}
	void cycle()
	{
		auto N = wsp_[A_] + wsp_[B_];
		if(N > 9)
			wsp_.push_back(N / 10);
		wsp_.push_back(N % 10);
		A_ += 1 + wsp_[A_];
		A_ %= wsp_.size();
		B_ += 1 + wsp_[B_];
		B_ %= wsp_.size();
	}
};

auto pt1(auto in)
{
	timer t("p1");
	recipe_list rl(in + 10);
	auto tgt = in + 10;
	while(rl.wsp_.size() < tgt)
		rl.cycle();
	std::string s;
	for(size_t a = tgt - 10; a != tgt; ++a)
		s.push_back(rl.wsp_[a] + '0');
	return s;
}

std::vector<uint8_t> to_v(int n)
{
	std::vector<uint8_t> v;
	while(n)
	{
		v.push_back(n % 10);
		n /= 10;
	}
	std::ranges::reverse(v);
	return v;
}

int pt2(auto in)
{
	timer t("p2");
	recipe_list rl(in + 10);
	const auto tgt = to_v(in);
	while(1)
	{
		rl.cycle();
		if(rl.wsp_.back() == tgt.back() && std::ranges::equal(tgt, std::ranges::subrange(std::ranges::end(rl.wsp_) - tgt.size(), std::ranges::end(rl.wsp_))))
			return rl.wsp_.size() - tgt.size();
		if(rl.wsp_[rl.wsp_.size() - 2] == tgt.back() && std::ranges::equal(tgt, std::ranges::subrange(std::ranges::end(rl.wsp_) - tgt.size() - 1, std::ranges::end(rl.wsp_) - 1)))
			return rl.wsp_.size() - tgt.size() - 1;
	}
	return rl.wsp_.size() - tgt.size();
}

int main(int ac, char** av)
{
	auto in = 540391;
	if(ac > 1)
		in = sv_to_t<int>(av[1]);
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
