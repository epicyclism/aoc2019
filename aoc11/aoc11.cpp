#include <iostream>
#include <string>
#include <tuple>

#include <fmt/format.h>
#include "ctre_inc.h"

#include "timer.h"

int power(int x, int y, int serial)
{
	auto rack_id = x + 10;
	auto power_level = rack_id * y + serial;
	power_level *= rack_id;
	power_level /= 100;
	power_level %= 10;
	return power_level - 5;
}

int grid(int x, int y, int z, int serial)
{
	int r = 0;
	for( int dx = 0; dx < z; ++dx)
		for(int dy = 0; dy < z; ++dy)
			r += power(x + dx, y + dy, serial);
	return r;
}

int grid_add(int x, int y, int z, int p, int serial)
{
	for(int dy = 0; dy < z; ++dy)
		p += power(x + z - 1, y + dy, serial);
	for(int dx = 0; dx < z; ++dx)
		p += power(x + dx, y + z - 1, serial);
	return p;
}

auto pt1(auto in)
{
	timer t("p1");
	int mx = 0;
	int my = 0;
	int mp = 0;
	for(int x = 1; x < 298; ++x)
		for(int y = 1; y < 298; ++y)
		{
			auto p = grid(x, y, 3, in);
			if( p > mp)
			{
				mp = p;
				mx = x;
				my = y;
			}
		}
	return std::make_pair(mx, my);
}

auto pt2(auto in)
{
	timer t("p2");
	int mx = 0;
	int my = 0;
	int mp = 0;
	int ms = 0;
	for(int x = 1; x < 301; ++x)
		for(int y = 1; y < 301; ++y)
		{
			auto p = grid(x, y, 1, in);
			for(int s = 2; s < std::min(301 - x, 301 - y); ++s)
			{
				p = grid_add(x, y, s, p, in);
				if( p > mp)
				{
					mp = p;
					mx = x;
					my = y;
					ms = s;
				}
			}
		}
	return std::make_tuple(mx, my, ms);
}

int main(int ac, char** av)
{
	auto in = 9798;
	if(ac > 1)
		in = sv_to_t<int>(av[1]);
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {},{}", p1.first, p1.second);
	fmt::println("pt2 = {},{},{}", std::get<0>(p2), std::get<1>(p2), std::get<2>(p2));
}
