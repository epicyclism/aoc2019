#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <set>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "timer.h"

constexpr std::array<int, 8> eight_round (int stride)
{
	return { -(stride + 1), -stride, -(stride - 1), -1, 1, stride - 1, stride, stride + 1};
}

auto get_input()
{
	std::vector<char> v;
	size_t stride = 0;
	std::string ln;
	if(std::getline(std::cin, ln))
	{
		stride = ln.size() + 2;
		v.insert(v.end(), stride, '.');
		v.emplace_back('.');
		v.insert(v.end(), ln.begin(), ln.end());
		v.emplace_back('.');
		while(std::getline(std::cin, ln))
		{
			v.emplace_back('.');
			v.insert(v.end(), ln.begin(), ln.end());
			v.emplace_back('.');			
		}
		v.insert(v.end(), stride, '.');
	}

	return std::make_pair(v, stride);
}

void print(std::vector<char> const& v, size_t stride, int step)
{
	fmt::println("step {} stride {}", step, stride);
	int cnt = 0;
	while(cnt < v.size())
	{
		fmt::println("{}", fmt::join(std::ranges::subrange(v.begin() + cnt,  v.begin() + cnt + stride), ""));
		cnt += stride;
	}
}

auto make_cnt(auto const& v, size_t p, auto const& r8)
{
	int t = 0;
	int l = 0;
	for(auto o : r8)
	{
		switch(v[p + o])
		{
			case '|':
				++t;
				break;
			case '#':
				++l;
				break;
			default:
				break;
		}
	}
	return std::make_pair(t, l);
}

void gen(std::vector<char> const& fr, std::vector<char>& to, size_t stride, std::array<int, 8> const& tx)
{
	for(int x = stride; x < fr.size() - stride;)
	{
		for(int p = 1; p < stride - 1; ++p)
		{
			auto pt = x + p;
			auto[t, l] = make_cnt(fr, pt, tx);
			switch(fr[pt])
			{
				case '.':
					to[pt] = t >= 3 ? '|' : '.';
					break;
				case '|':
					to[pt] = l >= 3 ? '#' : '|';						
					break;
				case '#':
					to[pt] = l && t ? '#' : '.';
					break;
			}
		}
		x += stride;
	}
}

auto pt12(auto const& in)
{
	timer tm("pt12");
	auto[g, stride] = in;
	std::vector<std::vector<char>> vv(700);
	for(auto& v: vv)
		v.resize(g.size(), '.');
	vv[0] = g;
	auto tx = eight_round(stride);
	for(int n = 1; n < vv.size(); ++n)
		gen(vv[n - 1], vv[n], stride, tx);
	// p1
	auto tc1 = std::ranges::count(vv[10], '|');
	auto lc1 = std::ranges::count(vv[10], '#');
	// p2
	int tc2 = -1;
	int lc2 = -1;
	for(int a = 0; a < vv.size() - 1; ++a)
		for(int b = a + 1; b < vv.size(); ++b)
			if(vv[a] == vv[b])
			{
				int n = (1000000000 - a) % (b - a);
				tc2 = std::ranges::count(vv[a + n], '|');
				lc2 = std::ranges::count(vv[a + n], '#');
				goto gotp2;
			}
gotp2:
	return std::make_pair(tc1 * lc1, tc2 * lc2);
}

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
