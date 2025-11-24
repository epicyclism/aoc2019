#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <bit>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

struct opcode_t
{
	char op_;
	int A_;
	int B_;
	int C_;
};

using reg_t = std::array<unsigned, 4>;

struct obs_t
{
	reg_t before_;
	opcode_t op_;
	reg_t after_;
};

auto get_input()
{
	std::vector<obs_t> obs;
	std::vector<opcode_t> prg;
	std::string ln;
	obs_t cache;
	int state = 0;
	while(std::getline(std::cin, ln))
	{
		if(ln.empty())
		{
			if(state == 2)
				break;
			++state;
		}
		else
		{
				state = 0;
			if(auto[m, a, b, c, d] = ctre::match<"Before: \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]">(ln); m)
				cache.before_ = {a.to_number<unsigned>(), b.to_number<unsigned>(), c.to_number<unsigned>(), d.to_number<unsigned>()};
			else
			if(auto[m, o, a1, b1, c1] = ctre::match<"(\\d+) (\\d+) (\\d+) (\\d+)">(ln); m)
				cache.op_ = opcode_t(o.to_number<unsigned>(), a1.to_number<unsigned>(), b1.to_number<unsigned>(), c1.to_number<unsigned>());
			else
			if(auto[m, a2, b2, c2, d2] = ctre::match<"After:  \\[(\\d+), (\\d+), (\\d+), (\\d+)\\]">(ln); m)
			{
				cache.after_ = {a2.to_number<unsigned>(), b2.to_number<unsigned>(), c2.to_number<unsigned>(), d2.to_number<unsigned>()};
				obs.emplace_back(std::move(cache));
			}
			else
				fmt::println("parse error {}", ln);
		}
	}
	while(std::getline(std::cin, ln))
	{
		if(auto[m, o, a, b, c] = ctre::match<"(\\d+) (\\d+) (\\d+) (\\d+)">(ln); m)
			prg.emplace_back(o.to_number<unsigned>(), a.to_number<unsigned>(), b.to_number<unsigned>(), c.to_number<unsigned>());
		else
			fmt::println("parse fail {}", ln);
	}
	return std::make_pair(obs, prg);
}

void addr(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] + r[op.B_];
}

void addi(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] + op.B_;
}

void mulr(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] * r[op.B_];
}

void muli(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] * op.B_;
}

void banr(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] & r[op.B_];
}

void bani(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] & op.B_;
}

void borr(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] | r[op.B_];
}

void bori(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] | op.B_;
}

void setr(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_];
}

void seti(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = op.A_;
}

void gtir(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = op.A_ > r[op.B_];
}

void gtri(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] > op.B_;
}

void gtrr(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] > r[op.B_];
}

void eqir(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = op.A_ == r[op.B_];
}

void eqri(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] == op.B_;
}

void eqrr(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[op.C_] = r[op.A_] == r[op.B_];
}

std::array ops = {addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr};

bool test_all(obs_t const& o)
{
	return std::ranges::count_if(ops, [&](auto op)
	{
		reg_t ro;
		ro = o.before_;
		op(o.op_, o.before_, ro);
		return ro == o.after_;
	}) >= 3;
}

int64_t pt1(auto const& in)
{
	timer t("p1");
	return std::ranges::count_if(in.first, [](auto& obs){return test_all(obs);});
}

bool test(char opcode, obs_t const& o)
{
	reg_t ro;
	ro = o.before_;
	ops[opcode](o.op_, o.before_, ro);
	return ro == o.after_;
}

uint16_t log(uint16_t u)
{
	uint16_t f = 1;
	uint16_t r = 0;
	while((f & u) != u)
	{
		f <<= 1;
		++r;
	}
	return r;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	std::array<uint16_t, 16> tab;
	tab.fill(0);
	uint16_t flg = 1;
	// test each actual instruction against each observation and mark the set that
	// fit
	for(uint8_t op = 0; op < 16; ++op)
	{
		for(auto& obs : in.first)
		{
			if(test(op, obs))
				tab[obs.op_.op_] |= flg;
		}
		flg <<= 1;
	}
	// decode the possibles to the onlys
	//
	std::array<uint8_t, 16> decode;
	decode.fill(-1);
	while(1)
	{
		auto i = std::ranges::find_if(tab, [](auto x){ return std::popcount(x) == 1;});
		if(i == std::ranges::end(tab))
			break;
		auto k = *i;
		decode[std::distance(std::ranges::begin(tab), i)] = log(k);
		for(auto& t : tab)
			t &= ~k;
	}
	// execute the program
	reg_t r;
	r.fill(0);
	for(auto& i: in.second)
		ops[decode[i.op_]](i, r, r);
	return r[0];
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
