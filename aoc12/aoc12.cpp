#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

using rep_set_t = std::array<char, 32>;

int to_key(std::string_view v)
{
	int rv = 0;
	for(auto c: v)
	{
		rv *= 2;
		rv += c == '#';
	}
	return rv;
}

auto get_input()
{
	std::vector<char> plants;
	rep_set_t instructions;
	std::string ln;
	std::getline(std::cin, ln);
	plants.resize(ln.size() - 15);
	std::ranges::copy(ln.begin() + 15, ln.end(), plants.begin());
	std::getline(std::cin, ln);
	while(std::getline(std::cin, ln))
	{
		if(auto[m, k, v] = ctre::match<"([\\.#]+) => ([\\.#])">(ln); m)
			instructions[to_key(k)] = v.view()[0];
		else
			fmt::println("parse fail at {}", ln);
	}
	return std::make_pair(plants, instructions);
}

void dmp(std::vector<char> const& plants,
	rep_set_t const& instructions)
{
	fmt::println("{}", fmt::join(plants, ""));
	for(int n = 0; n < 32; ++n)
		fmt::println("{}  {}", n, instructions[n]);
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto[plants, instructions] = in;
	std::vector<char> A(plants.size() + 40, '.');
	std::vector<char> B(plants.size() + 40, '.');
	std::ranges::copy(plants, A.begin() + 10);
	for(int gen = 0; gen < 20; ++gen)
	{
		int v = 0;
		int end = 4;
		for(int out = 2; out < B.size() - 3; ++out, ++end)
		{
			v *= 2;
			v += A[end] == '#';
			v &= 0x1f;
			B[out] = instructions[v];
		}
		A.swap(B);
		std::ranges::fill(B, '.');
	}
	int rv = 0;
	int pos = -10;
	for(auto c: A)
	{
		rv += (c == '#') ? pos : 0;
		++pos;
	}
	return rv;
}

using wsp = std::array<char, 256>;
constexpr int off = 16;

int64_t calc(wsp const& w, int64_t s)
{
	int64_t rv = 0;
	for(auto c: w)
	{
		rv += (c == '#') ? s : 0;
		++s;
	}
	return rv;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	auto[plants, instructions] = in;
	wsp A;
	A.fill( '.');
	wsp B;
	B.fill( '.');
	std::ranges::copy(plants, A.begin() + off);
	int fst = A.size();
	int last = 0;
	int pos = 0;
	int gen = 0;
	std::map<wsp, std::pair<int, int>> mp;
	mp.emplace(A, std::make_pair(gen, pos));
	while(1)
	{
		fst = A.size();
		last = 0;
		int v = 0;
		int end = 4;
		for(int out = 2; out < B.size() - 3; ++out, ++end)
		{
			v *= 2;
			v += A[end] == '#';
			v &= 0x1f;
			if(instructions[v] == '#')
			{
				if(out < fst)
					fst = out;
				B[out] = '#';
				last = out;
			}
		}
		pos += fst - 16;
		std::ranges::fill(A, '.');
		std::copy(B.begin() + fst, B.begin() + last + 1, A.begin() + off);
		std::ranges::fill(B, '.');
		++gen;
		if(mp.contains(A))
			break;
		mp.emplace(A, std::make_pair(gen, pos));
	}
	// assume the gap between repeats is one, stepped one to the right, once
	// stabilised.
	int64_t tp  = 50000000000LL - gen + pos - off;

	return calc(A, tp);
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}