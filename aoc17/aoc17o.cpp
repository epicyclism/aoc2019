#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "ctre_inc.h"
#include "timer.h"

struct in_t
{
	int a_;
	int f_;
	int t_;
	bool x_;
};

auto x_range(in_t const& t)
{
	return t.x_ ? std::make_pair(t.a_, t.a_ + 1) : std::make_pair(t.f_, t.t_ + 1);
}

auto y_range(in_t const& t)
{
	return t.x_ ? std::make_pair(t.f_, t.t_ + 1) : std::make_pair(t.a_, t.a_ + 1);
}

void offset(in_t& i, int x, int y)
{
	if(i.x_)
	{
		i.a_ += x;
		i.f_ += y;
		i.t_ += y;
	}
	else
	{
		i.a_ += y;
		i.f_ += x;
		i.t_ += x;
	}
}

auto get_input()
{
	std::vector<in_t> vi;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if (auto[m, xy, a, f, t] = ctre::match<"([xy])=(\\d+), .=(\\d+)\\.\\.(\\d+)">(ln); m)
			vi.emplace_back(a.to_number<int>(), f.to_number<int>(), t.to_number<int>(), xy.view()[0] == 'x');
	}
	int mny = std::numeric_limits<int>::max();
	int mnx = std::numeric_limits<int>::max();
	int mxy = 0;
	int mxx = 0;
	for(auto& i : vi)
	{
		auto[xf, xt] = x_range(i);
		mnx = xf < mnx ? xf : mnx;
		mxx = xt > mxx ? xt : mxx;
		auto[yf, yt] = y_range(i);
		mny = yf < mny ? yf : mny;
		mxy = yt > mxy ? yt : mxy;
	}
	mnx -= 1;
	mxx += 1;
	mny -= 1;
	for(auto& i : vi)
		offset(i, -mnx, -mny);
	mxy -= mny;
	mxx -= mnx;
	std::vector<char> v(mxy * mxx, '.');
	for(auto& i: vi)
	{
		auto[xf, xt] = x_range(i);
		auto[yf, yt] = y_range(i);
		for(auto y = yf; y < yt; ++y)
		{
			auto p = y * mxx;
			for(auto x = xf; x < xt; ++x)
				v[p + x] = '#';
		}	
	}
	return std::make_tuple(v, mxx, 500 - mnx);
}

void print (std::vector<char> const& v, int stride, int step)
{
	fmt::println("step {} stride {}", step, stride);
	int cnt = 0;
	while(cnt < v.size())
	{
		fmt::println("{} {}", cnt, fmt::join(std::ranges::subrange(v.begin() + cnt,  v.begin() + cnt + stride), ""));
		cnt += stride;
	}
}

using vertex_id_t = std::size_t;
constexpr vertex_id_t invalid_vertex_id = vertex_id_t(-1);

template<typename T> class grid_custom
{
private:
    const std::vector<T>& data_;
    const size_t stride_;
	int lr_;
public:
    grid_custom(std::vector<T> const& d, size_t s) : data_(d), stride_(s), lr_(0)
    {}
	void reset()
	{
		lr_ = 0;
	}
	// never more than one option! -1 for none
    vertex_id_t operator[](vertex_id_t v)
    {
		if(v + stride_ >= data_.size())
			return invalid_vertex_id; // can't look down, so no more.
		// down
        if (value(v + stride_) == '.')
		{
			lr_ = 0;
            return v + stride_;
		}
        // left/right if down is wall or water and left/right is space
        if ( (value(v + stride_) == '#' || value(v + stride_) == '~'))
		{
			if(value(v - 1) == '.' && value(v - 1 + stride_) != '.' && lr_ <= 0)
			{
				lr_ = -1;
            	return v - 1;
			}
			if ( value(v + 1) == '.' && value(v + 1 + stride_) != '.' && lr_ >= 0)
			{
				lr_ = 1;
				return v + 1;
			}
		}
 		return invalid_vertex_id;
    }
    size_t size() const
    {
        return data_.size();
    }
    size_t stride () const
    {
        return stride_;
    }
    T value(vertex_id_t v) const
    {
        return data_[v];
    }
};

int pt1(auto in)
{
	timer t("p1");
	auto& g = std::get<0>(in);
	auto str = std::get<1>(in);
	auto s = std::get<2>(in);
	print(g, str, 0);
	grid_custom gc(g, str);
	int cnt = 0;
//	while(1)
	while(cnt < 45)
	{
		auto v = gc[s];
		if(v == invalid_vertex_id)
			break;
		auto vp = v;
		v = s;
		gc.reset();
		while(v != invalid_vertex_id)
		{
			vp = v;
			v = gc[vp];
//			fmt::println("{} -> {}", vp, v);
		}
		g[vp] = '~';
		++cnt;
		print(g, str, cnt);
	}
//	print(g, str, cnt);
	return cnt;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	return 0;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
