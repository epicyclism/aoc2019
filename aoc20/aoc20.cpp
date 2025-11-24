#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <queue>

#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "timer.h"

auto get_input()
{
	std::string ln;
	std::getline(std::cin, ln);
	return ln;
}

using pt_t = std::pair<int, int>;

pt_t shift(pt_t p, int x, int y)
{
	p.first += x;
	p.second += y;
	return p;
};

pt_t operator+(pt_t l, pt_t const& r)
{
	l.first += r.first;
	l.second += r.second;
	return l;
}

// this cannot be best?
constexpr std::array<pt_t, 4> cardinals {pt_t(1, 0), pt_t(-1, 0), pt_t(0, 1), pt_t(0, -1)};

using door_t = std::pair<pt_t, pt_t>;

door_t door(pt_t const& a, pt_t const& b)
{
	return {std::min(a, b), std::max(a, b)};
}

std::set<door_t> all_doors;

pt_t move(pt_t p, char d)
{
	switch(d)
	{
		case 'E':
			return shift(p, 1, 0);
		case 'N':
			return shift(p, 0, -1);
		case 'S':
			return shift(p, 0, 1);
		case 'W':
			return shift(p, -1, 0);
	}
	return shift(p, 0, 0);
}

struct base_elt
{
	virtual std::set<pt_t> follow(std::set<pt_t> const& in) = 0;
};

struct letter_t : base_elt
{
	char letter_;
	letter_t(char l) : letter_(l)
	{}
	virtual std::set<pt_t> follow(std::set<pt_t> const& in)
	{
		std::set<pt_t> out;
		for(auto& rm : in)
		{
			auto nxt = move(rm, letter_);
			all_doors.emplace(door(rm, nxt));
			out.insert(nxt);
		}
		return out;
	}
};

struct empty_t : base_elt
{
	virtual std::set<pt_t> follow(std::set<pt_t> const& in)
	{
		return in;
	}
};

struct or_t : base_elt
{
	base_elt* l_;
	base_elt* r_;
	or_t(base_elt* l, base_elt* r) : l_(l), r_(r)
	{}
	virtual std::set<pt_t> follow(std::set<pt_t> const& in)
	{
		auto ls = l_->follow(in);
		auto rs = r_->follow(in);
		ls.insert_range(rs);
		return ls;
	}
};

struct and_t : base_elt
{
	base_elt* l_;
	base_elt* r_;
	and_t(base_elt* l, base_elt* r) : l_(l), r_(r)
	{}
	virtual std::set<pt_t> follow(std::set<pt_t> const& in)
	{
		auto ls = l_->follow(in);
		return r_->follow(ls);
	}
};

class parser_t
{
private:
	std::string_view rx_;
	size_t idx_;

	bool end() const
	{
		return !(idx_ < rx_.size());
	}
	bool expect(char c)
	{
		if(end())
			return false;
		if(rx_[idx_] == c)
		{
			++idx_;
			return true;
		}
		return false;
	}
	char current()
	{
		return rx_[idx_++];
	}
	char peek() const
	{
		if(end())
			return 0;
		return rx_[idx_];
	}

	base_elt* parse_bit()
	{
		if(expect('('))
		{
			auto* p = parse();
			expect(')');
			return p;
		}
		return new letter_t(current());
	}

	base_elt* parse_and()
	{
		base_elt* p = new empty_t();
		while(peek() != 0 && peek() != ')' && peek() != '|')
		{
			auto* pn = parse_bit();
			p = new and_t(p, pn);
		}
		return p;
	}

public:
	parser_t(std::string_view s) : rx_(s), idx_(0)
	{}
	base_elt* parse()
	{
		auto* p = parse_and();
		while(expect('|'))
		{
			auto* pn = parse_and();
			p = new or_t(p, pn);
		}
		return p;
	}
};

auto pt12(auto const& in)
{
	timer t("p12");
	all_doors.clear();
	parser_t p(in);
	auto* root = p.parse();
	std::set<pt_t> X;
	X.emplace(0, 0);
	root->follow(X);
	std::queue<pt_t> q;
	std::map<pt_t, unsigned> d;
	q.push({0, 0});
	while(!q.empty())
	{
		auto e = q.front();
		q.pop();
		for(auto c: cardinals)
		{
			auto n = e + c;
			if(d.contains(n))
				continue;
			auto dr = door(e, n);
			if(all_doors.contains(dr))
			{
				d[n] = d[e] + 1;
				q.push(n);
			}
		}
	}
	unsigned p1 = 0;
	unsigned p2 = 0;
	for(auto& e: d)
	{
		p1 = std::max(p1, e.second);
		if(e.second >= 1000)
			++p2;
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
