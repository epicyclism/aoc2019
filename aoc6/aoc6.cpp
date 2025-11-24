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

struct pt
{
	int x_;
	int y_;
};

auto scan(auto const& in)
{
	pt tl(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	pt br(0, 0);
	for(auto p : in)
	{
		if(p.x_ < tl.x_)
			tl.x_ = p.x_;
		if(p.x_ > br.x_)
			br.x_ = p.x_;
		if(p.y_ < tl.y_)
			tl.y_ = p.y_;
		if(p.y_ > br.y_)
			br.y_ = p.y_;
	}
	return std::make_pair(tl, br);
}

auto get_input()
{
	std::vector<pt> v;
	std::string ln;
	while(std::getline(std::cin, ln))
		if(auto[m, x, y] = ctre::match<"(\\d+), (\\d+)">(ln); m)
			v.emplace_back(x.to_number<int>(), y.to_number<int>());
		else
			fmt::println("parse fail: {}", ln);
	auto[tr, bl] = scan(v);
	std::ranges::transform(v, std::ranges::begin(v), [&](auto p){ return pt(p.x_ - tr.x_, p.y_ - tr.y_);});
	return v;
}


int manhattan(pt f, pt t)
{
	return std::abs(f.x_ - t.x_) + std::abs(f.y_ - t.y_);
}

struct cell
{
	int owned_ = -1;
	int distance_ = std::numeric_limits<int>::max();
	int total_d_ = 0;
};

auto pt12(auto const& in)
{
	timer t("p12");
	auto[tl, br] = scan(in);
	int mxx = br.x_ + 1;
	int mxy = br.y_ + 1;
	std::vector<cell> vc(mxx * mxy);
	for(int y = 0; y < mxy; ++y)
	{
		for(int x = 0; x < mxx; ++x)
		{
			int ca = y * mxx + x;
			int t = 0;
			for(int n = 0; n < in.size(); ++n)
			{
				auto d = manhattan(in[n], {x, y} );
				t += d;
				if( d < vc[ca].distance_)
				{
					vc[ca].distance_ = d;
					vc[ca].owned_ = n;
				}
				else
				if(d == vc[ca].distance_)
					vc[ca].owned_ = -1;
			}
			vc[ca].total_d_ = t;
		}
	}
	std::vector<bool> edge(in.size());
	for(int x = 0; x < mxx; ++x)
	{
		edge[vc[x].owned_] = true;
		edge[vc[x + (mxy - 1) * mxx].owned_] = true;
	}
	for(int y = 0; y < mxy; ++y)
	{
		edge[vc[y * mxx].owned_] = true;
		edge[vc[y * mxx + mxy - 1].owned_] = true;
	}
	int p1 = 0;
	for(int c = 0; c < in.size(); ++c)
	{
		if(!edge[c])
		{
			auto mxt = std::ranges::count_if(vc, [=](auto& cl){ return cl.owned_ == c;});
			if(mxt > p1)
				p1 = mxt;
		}
	}
	auto thres = (in.size() < 10 ) ? 32 : 10000;
	auto p2 = std::ranges::count_if(vc, [=](auto& cl){ return cl.total_d_ < thres;});
	return std::make_pair(p1, p2);
}

int main()
{
	auto in = get_input();
	auto[ p1, p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
