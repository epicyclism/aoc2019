#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <string>

#include "ctre.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

struct intcode_wsp;
void plus(intcode_wsp& ic);
void times(intcode_wsp& ic);
void input(intcode_wsp& ic);
void output(intcode_wsp& ic);
void jump_if_true(intcode_wsp& ic);
void jump_if_false(intcode_wsp& ic);
void less_than(intcode_wsp& ic);
void equals(intcode_wsp& ic);
void set_relative_base(intcode_wsp& ic);
void invalid(intcode_wsp& ic);
void end(intcode_wsp&);

struct intcode_wsp
{
	size_t pc_ = 0;
    int64_t rel_base_ = 0;
	std::vector<int64_t> mem_;
	void (*acts_[100])(intcode_wsp&) =
	{ 	invalid,    plus,   times,   input,  output, jump_if_true, jump_if_false, less_than, equals, set_relative_base,
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
    std::queue<int64_t> in_;
    int64_t buf_;
    bool halted_ = false;
    std::array<uint8_t, 4> modes()
    {
        std::array<uint8_t, 4> m;
        m.fill(0);
        auto i = mem_[pc_] / 100;
        auto mi = m.begin() + 1;
        while(i)
        {
            *mi = i % 10;
            i /= 10;
            ++mi;
        }
//        fmt::println("{}", m);
        return m;
    }
    int64_t argi(uint8_t n,  std::array<uint8_t, 4> const& m)
    {
        switch(m[n])
        {
            case 0:
                return mem_[mem_[pc_ + n]];
            case 1:
                return mem_[pc_ + n];
            case 2:
                return mem_[mem_[pc_ + n] + rel_base_];
            default:
                break;
        }
        return 0;
    }
	void execute()
	{
        halted_ = false;
        mem_.resize(1000000);
		acts_[mem_[pc_] % 100](*this);
	}
    void reset()
    {
        halted_ = false;
        pc_ = 0;
        rel_base_ = 0;
        buf_ = 0;
        while(!in_.empty())
            in_.pop();
    }
};

void plus(intcode_wsp& ic)
{
    auto m = ic.modes();
    int64_t a = ic.mem_[ic.pc_ + 3];
	ic.mem_[a] = ic.argi(1, m) + ic.argi(2, m);
	ic.pc_ += 4;
	ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void times(intcode_wsp& ic)
{
    auto m = ic.modes();
    int64_t a = ic.mem_[ic.pc_ + 3];
	ic.mem_[a] = ic.argi(1, m) * ic.argi(2, m);
	ic.pc_ += 4;
	ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void input(intcode_wsp& ic)
{
    auto m = ic.modes();
    int64_t a = ic.argi(1, m);
    ic.mem_[a] = ic.in_.front();
    ic.in_.pop();
    ic.pc_ += 2;
    ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void output(intcode_wsp& ic)
{
    auto m = ic.modes();
    ic.buf_ = ic.argi(1, m);
    fmt::println("{} {}", m, ic.buf_);
    ic.pc_ += 2;
//    ic.acts_[ic.mem_[ic.pc_] % 100](ic);
    return;
}

void jump_if_true(intcode_wsp& ic)
{
    auto m = ic.modes();
    if(ic.argi(1, m))
        ic.pc_ = ic.argi(2, m);
    else
        ic.pc_ += 3;
    ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void jump_if_false(intcode_wsp& ic)
{
    auto m = ic.modes();
    if(!ic.argi(1, m))
        ic.pc_ = ic.argi(2, m);
    else
        ic.pc_ += 3;
    ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void less_than(intcode_wsp& ic)
{
    auto m = ic.modes();
    int64_t out = ic.argi(1, m) < ic.argi(2, m);
    int64_t a = ic.mem_[ic.pc_ + 3];
    ic.mem_[a] = out;
    ic.pc_ += 4;
    ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void equals(intcode_wsp& ic)
{
    auto m = ic.modes();
    int64_t out = ic.argi(1, m) == ic.argi(2, m);
    int64_t a = ic.mem_[ic.pc_ + 3];
    ic.mem_[ic.mem_[ic.pc_ + 3]] = out;
    ic.pc_ += 4;
    ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void set_relative_base(intcode_wsp& ic)
{
    auto m = ic.modes();
    fmt::print("rb {} -> ", ic.rel_base_);
    ic.rel_base_ += ic.argi(1, m);
    fmt::println("{}", ic.rel_base_);
    ic.pc_ += 2;
    ic.acts_[ic.mem_[ic.pc_] % 100](ic);
}

void invalid(intcode_wsp& ic)
{
	fmt::println("{}", ic.mem_);
    fmt::println("pc = {}, inst = {}", ic.pc_, ic.mem_[ic.pc_]);
	fmt::println("bogus instruction");
	return;
}

void end(intcode_wsp& ic)
{
    ic.halted_ = true;
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