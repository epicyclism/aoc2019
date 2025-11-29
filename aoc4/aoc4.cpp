#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

using code_t = std::array<char, 6>;

void print(code_t const& c)
{
	fmt::println("{}", fmt::join(c, ","));
}
void printx(code_t const& c)
{
	fmt::print("{{");
	for(auto cc : c)
		fmt::print("{}", +cc);
	fmt::println("}}");
}

void inc(code_t& c)
{
	int digit = 5;
	while(digit >= 0)
	{
		if(c[digit] < '9')
		{
			++c[digit];
			break;
		}
		c[digit] = '0';
		--digit;
	}
}

bool valid(code_t const& c)
{
	return std::ranges::adjacent_find(c) != std::ranges::end(c) &&
			std::ranges::is_sorted(c);
}

// aargh!
bool valid2(code_t const& c)
{
	code_t cc;
	cc.fill(1);
	int i0 = 0;
	int i1 = 1;
	
	while( i1 < c.size())
	{
		if(c[i0] == c[i1])
			cc[i1] = cc[i0] + 1;
		++i0;
		++i1;
	}
	i0 = 0;
	i1 = 1;
	while(i0 < c.size())
	{
		if(cc[i0] == 2)
		{
			if(i1 == cc.size() || cc[i1] == 1)
				return true;
		}
		++i0;
		++i1;
	}
	return false;
}

auto parse_input(std::string_view s)
{
	code_t f;
	code_t t;
	auto p = s.find('-');
	std::copy(s.begin(), s.begin() + 6, f.begin());
	std::copy(s.begin() + 7, s.end(), t.begin());
	return std::make_pair(f, t);
}

auto pt12(code_t f, code_t t)
{
	timer tm("p12");
	int cnt1 = 0;
	int cnt2 = 0;
	while(f != t)
	{
		if(valid(f))
		{
			++cnt1;
			cnt2 += valid2(f);
		}
		inc(f);
	}
	return std::make_pair(cnt1, cnt2);
}

int main(int ac, char** av)
{
	std::string_view in = "145852-616942";
	if(ac > 1)
		in = av[1];
	auto[f, t] = parse_input(in);
	auto[p1, p2] = pt12(f, t);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
