#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "timer.h"

struct cart_t
{
	int x_;
	int y_;
	char c_;
	int st_;
	int id_;
};

struct field_t
{
	int stride_;
	std::vector<char> v_;
};

auto find_carts(field_t& f)
{
	std::vector<cart_t> cts;
	int y = 0;
	int x = 0;
	int n = 0;
	for(auto& c : f.v_)
	{
		switch(c)
		{
			case '<':
			case '>':
				cts.emplace_back(x, y, c, 0, n++);
				c = '-';
				break;
			case '^':
			case 'v':
				cts.emplace_back(x, y, c, 0, n++);
				c = '|';
				break;
		}
		++x;
		if( x == f.stride_)
		{
			++y;
			x = 0;
		}
	}
	return cts;
}

auto get_input()
{
	field_t f;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		f.stride_ = ln.size();
		f.v_.append_range(ln);
	}

	return f;
}

char left(char c)
{
	switch(c)
	{
		case '<':
			return 'v';
		case 'v':
			return '>';
		case '>':
			return '^';
		case '^':
			return '<';
		default:
			fmt::println("left bug {}", c);
			break;
	}
	return '=';
}

char right(char c)
{
	switch(c)
	{
		case '<':
			return '^';
		case '^':
			return '>';
		case '>':
			return 'v';
		case 'v':
			return '<';
		default:
			fmt::println("right bug {}", c);
			break;
	}
	return '=';
}

void turn(field_t const& f, cart_t& c)
{
	int cur = c.y_ * f.stride_ + c.x_;
	switch(f.v_[cur])
	{
		case '|':
		case '-': // assume correctness of map
			break;
		case '\\':
			switch(c.c_)
			{
				case '>':
					c.c_ = 'v';
					break;
				case '<':
					c.c_ = '^';
					break;
				case '^':
					c.c_ = '<';
					break;
				case 'v':
					c.c_ = '>';
					break;
			}
			break;
		case '/':
			switch(c.c_)
			{
				case '>':
					c.c_ = '^';
					break;
				case '<':
					c.c_ = 'v';
					break;
				case '^':
					c.c_ = '>';
					break;
				case 'v':
					c.c_ = '<';
					break;
			}
			break;
		case '+':
			switch(c.st_)
			{
				case 0:
					c.c_ = left(c.c_);
					c.st_ = 1;
					break;
				case 1:
					c.st_ = 2;
					break;
				case 2:
					c.c_ = right(c.c_);
					c.st_ = 0;
					break;
			}
			break;
		default:
			fmt::println("turn bug {}", c.c_);
			break;
	}
}

void move_cart(field_t const& f, cart_t& c)
{
	switch(c.c_)
	{
		case '<':
			--c.x_;
			break;
		case '>':
			++c.x_;
			break;
		case '^':
			--c.y_;
			break;
		case 'v':
			++c.y_;
			break;
		default:
			fmt::println("move_cart bug {}", c.c_);
			break;
	}
	turn(f, c);
}

auto pt12(auto in)
{
	timer t("p13");
	std::string p1;
	std::string p2;
	auto cs = find_carts(in);
	while(1)
	{
		for(auto& c: cs )
		{
			if(c.id_ != -1)
			{
				move_cart(in, c);
				for(auto& c2 : cs)
				{
					if(c2.id_ != c.id_ && c2.x_ == c.x_ && c2.y_ == c.y_)
					{
						if(p1.empty())
							p1 = std::format("{},{}", c.x_, c.y_);
						c.id_ = -1;
						c.y_ = 500;
						c2.y_ = 500;
						c2.id_ = -1;
					}
				}
			}
		}
		std::ranges::sort(cs, [](auto& l, auto& r)
									{
										if(l.y_ == r.y_)
											return l.x_ < r.x_;
										return l.y_ < r.y_;
									});
		if(std::ranges::count(cs, -1, &cart_t::id_) > cs.size() - 3)
			break;
	}
	p2 = fmt::format("{},{}", cs[0].x_, cs[0].y_);
	return std::make_pair(p1, p2);
}

int main()
{
	auto in = get_input();
	auto[p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
