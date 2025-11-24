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
//	mxy += 1;
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
		fmt::println("{:>6} {}", cnt, fmt::join(std::ranges::subrange(v.begin() + cnt,  v.begin() + cnt + stride), ""));
		cnt += stride;
	}
}

using vertex_id_t = std::size_t;
constexpr vertex_id_t invalid_vertex_id = vertex_id_t(-1);

template<typename T> class grid_custom
{
private:
    std::vector<T>& data_;
    const size_t stride_;
	int lr_;
public:
    grid_custom(std::vector<T>& d, size_t s) : data_(d), stride_(s), lr_(0)
    {}
	void reset()
	{
		lr_ = 0;
	}
	vertex_id_t up(vertex_id_t f)
	{
		if(f < stride_)
			return invalid_vertex_id;
		return f - stride_;
	}
	vertex_id_t down(vertex_id_t f)
	{
		if(f + stride_ >= data_.size())
			return invalid_vertex_id;
		return f + stride_;
	}
	// assume straight down is already precluded...
	std::pair<vertex_id_t, bool> left(vertex_id_t f)
	{
		auto d = down(f);
		if(d == invalid_vertex_id)
			return {f, false};
		auto l = f;
		while(data_[l] != '#' && data_[d] != '.' && data_[d] != '|' )
		{
			--l;	
			d = down(l);
		}
		if(data_[l] == '#')
			return {l + 1, true};
		return {l, false};
	}
	std::pair<vertex_id_t, bool> right(vertex_id_t f)
	{
		auto d = down(f);
		if(d == invalid_vertex_id)
			return {f, false};
		auto r = f;
		while(data_[r] != '#' && data_[d] != '.' && data_[d] != '|' )
		{	
			++r;
			d = down(r);
		}
		if(data_[r] == '#')
			return {r - 1, true};
		return {r, false};		
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
	void set_value(vertex_id_t v, T val)
	{
		data_[v] = val;
	}
};

void drip_and_fill(auto& gc, vertex_id_t drip_from)
{
	auto  d = drip_from;
	auto dn = gc.down(drip_from);
	while( dn != invalid_vertex_id && gc.value(dn) == '.')
	{
		d = dn;
		dn = gc.down(dn);
	}
	int lc = -1;
	int rc = -1;
	while(d != drip_from)
	{
		auto[lp, lw] = gc.left(d);
		auto[rp, rw] = gc.right(d);
		if(lw && rw) 
		{
			for(; lp <= rp; ++lp)
				gc.set_value(lp, '~');
			d = gc.up(d);
		}
		else
		{
			for(auto l = lp; l <= rp; ++l)
				gc.set_value(l, '|');
			if(lp != rp && !(lp == lc && rp == rc))
			{
				if(!lw)
					drip_and_fill(gc, lp);
				if(!rw)
					drip_and_fill(gc, rp);
				lc = lp;
				rc = rp;
			}
			else
			{
				lc = -1;
				rc = -1;
				d = gc.up(d);
			}
		}
	}
}

auto pt12(auto in)
{
	timer t("p12");
	auto& g = std::get<0>(in);
	auto str = std::get<1>(in);
	auto s = std::get<2>(in);
	grid_custom gc(g, str);
	drip_and_fill(gc, s);
//	print(g, str, 1);
	auto p = std::ranges::count_if(g, [](auto c){ return c == '|';});
	auto w = std::ranges::count_if(g, [](auto c){ return c == '~';});

	return std::make_pair(p + w, w);
}

int main()
{
	auto in = get_input();
	auto[p1,p2] = pt12(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
