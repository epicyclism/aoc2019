#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

struct line_t
{
	int x1_;
	int y1_;
	int d1_;
	int x2_;
	int y2_;
	int d2_;
};

std::vector<line_t> proc_line(std::string_view s)
{
	std::vector<line_t> v;
	int x = 0;
	int y = 0;
	int d = 0;
	int xn = 0;
	int yn = 0;
	int dn = 0;
	for(auto[m, di, r] : ctre::search_all<"([DLRU])(\\d+)">(s))
	{
		int rd = r.to_number<int>();
		dn += rd;
		switch(di.to_view().front())
		{
			case 'D':
				yn = y - rd;
				break;
			case 'L':
				xn = x - rd;
				break;
			case 'R':
				xn = x + rd;
				break;
			case 'U':
				yn = y + rd;
				break;
		}
		v.emplace_back(x, y, d, xn, yn, dn);
		x = xn;
		y = yn;
		d = dn;
	}
	return v;
}

auto get_input()
{
	std::string ln1;
	std::getline(std::cin, ln1);
	std::string ln2;
	std::getline(std::cin, ln2);

	return std::make_pair(proc_line(ln1), proc_line(ln2));
}

std::pair<int, int> clean(double x, double y)
{
	return { static_cast<int>(std::abs(x) + 0.5), static_cast<int>(std::abs(y) + 0.5)};
}

int md(int x, int y, int x1 = 0, int y1 = 0)
{
	return std::abs(x - x1) + std::abs(y - y1);
}

std::optional<std::pair<double, double>> intersect(line_t const& a, line_t const& b)
{
	double t = double((a.x1_ - b.x1_)*(b.y1_ - b.y2_) - (a.y1_ - b.y1_)*(b.x1_ - b.x2_) )
						 / ((a.x1_ - a.x2_)*(b.y1_ - b.y2_) - (a.y1_ - a.y2_)*(b.x1_ - b.x2_));
	double u = double((a.x1_ - a.x2_)*(a.y1_ - b.y1_) - (a.y1_ - a.y2_)*(a.x1_ - b.x1_) )
						 / ((a.x1_ - a.x2_)*(b.y1_ - b.y2_) - (a.y1_ - a.y2_)*(b.x1_ - b.x2_));
	u *= -1.0;
	if( t < 0.0 || t > 1.0 || u < 0.0 || u > 1.0)
		return std::nullopt;
	double px = a.x1_ + t * (a.x2_ - a.x1_);
	double py = a.y1_ + t * (a.y2_ - a.y1_);
	return std::optional<std::pair<double, double>>(std::in_place, px, py);
}

std::pair<int, int> pt12(auto const& in)
{
	timer t("p12");
	auto[r1, r2] = in;
	int p1 = std::numeric_limits<int>::max() ;
	int p2 = std::numeric_limits<int>::max() ;
//	for(auto[l1, l2] : std::views::cartesian_product(r1, r2))
	for(auto const& l1: r1)
		for(auto const& l2: r2)	{
			if(auto i = intersect(l1, l2); i)
			{
				auto[x, y] = clean(i.value().first, i.value().second);
				auto d = md(x, y);
				if(d < p1 && d > 0)
					p1 = d;
				auto d2 = l1.d1_ + md(x, y, l1.x1_, l1.y1_) + 
							l2.d1_ + md(x, y, l2.x1_, l2.y1_);
				if(d2 < p2 && d > 0)
					p2 = d2;
			}
	}
	return {p1, p2};
}

int main()
{
	auto in = get_input();
	auto [p1, p2] = pt12(in);
	fmt::println("pt1  = {}", p1);
	fmt::println("pt2  = {}", p2);
}
