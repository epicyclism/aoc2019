#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <set>
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

using reg_t = std::array<unsigned, 6>;

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


struct executor
{
	std::vector<opcode_t> prg_;
	reg_t r_;
	int ipr_;

	executor(std::vector<opcode_t> const& p, int ipr) : prg_(p), ipr_(ipr)
	{}

	std::pair<int64_t, int64_t> execute()
	{
		std::set<int64_t> rs;
		r_.fill(0);
		unsigned ip = 0;
		int64_t cnt = 0;
		int tgt_r = prg_[28].A_;
		int64_t rp = 0;
		int64_t r1 = 0;
		while(ip < prg_.size() && cnt < 10000000000)
		{
			r_[ipr_] = ip;
			auto& o = prg_[ip];
			ops[o.op_](o, r_, r_);
			ip = r_[ipr_];
			++ip;
			++cnt;
			if(ip == 28)
			{
				if(r1 == 0)
					r1 = r_[tgt_r];
				if(rs.contains(r_[tgt_r]))
					goto gotem;
				rp = r_[tgt_r];
				rs.insert(rp);
			}
		}
gotem:
		return {r1, rp};
	}
};

auto pt12(auto const& in)
{
	timer t("p12");
	executor ex(in.second, in.first);
	return ex.execute();
}

// 14108411 too high p2

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
