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
	int64_t A_;
	int64_t B_;
	int64_t C_;
};

char to_op(std::string_view s)
{
	constexpr std::array opcodes{"addr", "addi", "mulr", "muli", "banr", "bani", "borr", "bori", "setr", "seti", "gtir", "gtri", "gtrr", "eqir", "eqri", "eqrr"};
	return std::ranges::find(opcodes, s) - std::ranges::begin(opcodes);
}

using reg_t = std::array<unsigned, 7>;

auto get_input()
{
	std::vector<opcode_t> prg;
	std::string ln;
	int ip = 0;
	while(std::getline(std::cin, ln))
	{
		if(auto[m, o, a, b, c] = ctre::match<"(\\w+) (\\d+) (\\d+) (\\d+)">(ln); m)
			prg.emplace_back(to_op(o.view()), a.to_number<unsigned>(), b.to_number<unsigned>(), c.to_number<unsigned>());
		else
		if(auto[m, r] = ctre::match<"#ip (\\d+)">(ln); m)
			ip = r.to_number<unsigned>();
		else
			fmt::println("parse fail {}", ln);
	}
	return std::make_pair(ip, prg);
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

void ip(opcode_t const& op, reg_t const& r, reg_t& ro)
{
	ro[6] = op.A_;
}

std::array ops = {addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr,ip};

int64_t pt1(auto const& in)
{
	timer t("p1");
	reg_t r;
	r.fill(0);
	r[6] = in.first;
	unsigned ip = 0;
	int cnt = 0;
	while(ip < in.second.size())
	{
		r[r[6]] = ip;
		auto& o = in.second[ip];
		ops[o.op_](o, r, r);
		ip = r[r[6]];
		++ip;
		++cnt;
	}
	return r[0];
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = no chance, try the other");
}