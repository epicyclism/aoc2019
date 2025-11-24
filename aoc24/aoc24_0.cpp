#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

constexpr unsigned radiation = 0x00000001;
constexpr unsigned bludgeoning = 0x00000002;
constexpr unsigned slashing = 0x00000004;
constexpr unsigned fire = 0x00000008;
constexpr unsigned cold = 0x00000010;

unsigned scan_types(std::string_view s)
{
	unsigned t = 0;
	if(s.contains("radiation"))
		t |= radiation;
	if(s.contains("bludgeoning"))
		t |= bludgeoning;
	if(s.contains("slashing"))
		t |= slashing;
	if(s.contains("fire"))
		t |= fire;
	if(s.contains("cold"))
		t |= cold;
//	fmt::println("{} : {:x}", s, t);
	return t;
}

struct group_t
{
	int units_ = 0;
	int hit_points_ = 0;
	int attack_damage_ = 0;
	int initiative_ = 0;
	unsigned attacktype_ = 0;
	unsigned immunities_ = 0;
	unsigned weaknesses_ = 0;
	int tgt_ = -1;
	bool tgtd_ = false;
	bool infection_ = false;
};

using system_t = std::vector<group_t>;

int effective_damage(group_t const& g)
{
	return g.units_ * g.attack_damage_;
}

void reset(group_t& g)
{
	g.tgtd_ = false;
	g.tgt_ = -1;
}

void reset(system_t& s)
{
	for(auto& g: s)
		reset(g);
}

void proc_group(std::string const& ln, system_t& gs, bool infection)
{
	group_t g;
	g.infection_ = infection;
	if(auto[m, u, h] = ctre::starts_with<"(\\d+) units each with (\\d+) hit points">(ln); m)
	{
		g.units_ = u.to_number<int>();
		g.hit_points_ = h.to_number<int>();
	}
	else
		fmt::println("parse fail 1, {}", ln);
	if(auto[m, i] = ctre::search<"immune to ([^;\\)]+)">(ln); m)
		g.immunities_ = scan_types(i);
	if(auto[m, w] = ctre::search<"weak to ([^\\)]+)">(ln); m)
		g.weaknesses_ = scan_types(w);
	if(auto[m, d, t, i] = ctre::search<"with an attack that does (\\d+) (\\w+) damage at initiative (\\d+)">(ln); m)
	{
		g.attack_damage_ = d.to_number<int>();
		g.attacktype_ = scan_types(t);
		g.initiative_ = i.to_number<int>();
	}
	else
		fmt::println("parse fail 4, {}", ln);
	gs.push_back(g);
}

auto get_input()
{
	system_t ii;
	int current = 0;
	std::string ln;
	while(std::getline(std::cin, ln))
	{
		if(ln.empty())
			continue;
		if(ln == "Immune System:")
			current = 0;
		else
		if(ln == "Infection:")
			current = 1;
		else
			proc_group(ln, ii, current);
	}

	return ii;
}

void dump(system_t const& st, int rnd)
{
	fmt::println("round {}", rnd);
	for(auto g: st)
		fmt::println("{} : {} {} {} {} {:x} {:x} {:x}", g.infection_ ? "infection" : "immune   ", g.units_, g.hit_points_, g.attack_damage_, g.initiative_, g.attacktype_, g.immunities_, g.weaknesses_);
}

bool fighting(system_t const& s)
{
	bool immune = false;
	bool infect = false;
	for(auto& g: s)
	{
		if(g.units_ > 0)
		{
			if(g.infection_)
			{
				if(immune)
					return true;
				infect = true;
			}
			else
			{
				if(infect)
					return true;
				immune = true;
			}
		}
	}
	return false;
}

void build(system_t const& st, std::vector<int>& v, bool infect)
{
	v.clear();
	int n = 0;
	for(auto const& g : st)
	{
		if(g.infection_ == infect)
			v.push_back(n);
		++n;
	}
}

int calculate_damage(group_t const& attacker, group_t const& defender)
{
	if(attacker.attacktype_ & defender.immunities_)
		return 0;
	if(attacker.units_ == 0 || defender.units_ == 0)
		return 0;
	int base = effective_damage(attacker);
	if( attacker.attacktype_ & defender.weaknesses_)
		base *= 2;

	return base;
}

void choose_target(group_t& attacker, std::vector<int>& tgts, system_t& in)
{
	if(attacker.units_ < 1 || attacker.tgt_ != -1)
		return;
	// sort tgts by damage from this attacker.
	std::ranges::sort(tgts, [&](auto l, auto r)
		{
			auto dl = calculate_damage(attacker, in[l]);
			auto dr = calculate_damage(attacker, in[r]);
			if(dl == dr)
			{
				auto epl = effective_damage(in[l]);
				auto epr = effective_damage(in[r]);
				if(epl == epr)
					return in[l].initiative_ > in[r].initiative_;
				return epl > epr;
			}
			return dl > dr; 
		});
	for(auto t: tgts)
	{
		if(!in[t].tgtd_ && in[t].units_)
		{
			attacker.tgt_ = t;
			in[t].tgtd_ = true;
			return;
		}
	}
}

int64_t pt1(auto in)
{
	timer t("p1");
	int rnd = 0;
	dump(in, rnd);
	std::vector<int> attack_order;
	std::vector<int> init_order;
	attack_order.resize(in.size());
	init_order.resize(in.size());
	std::iota(std::ranges::begin(attack_order), std::ranges::end(attack_order), 0);
	std::iota(std::ranges::begin(init_order), std::ranges::end(init_order), 0);
	std::ranges::sort(init_order, [&](auto l, auto r)
			{
				auto& lg = in[l];
				auto& rg = in[r];
				return lg.initiative_ > rg.initiative_;
			});
	std::vector<int> infection;
	std::vector<int> immune;
	build(in, immune, false);
	build(in, infection, true);

	while(fighting(in))
//	while(rnd < 40)
	{
		reset(in);
		// select targets in decreasing order of effective power
		std::ranges::sort(attack_order, [&](auto l, auto r)
			{
				auto& lg = in[l];
				auto& rg = in[r];
				auto el = effective_damage(lg); auto er = effective_damage(rg);
				if(el == er)
					return lg.initiative_ > rg.initiative_;
				return el > er;
			});
		for(auto n: attack_order)
		{
			auto& g = in[n];
			choose_target(g, g.infection_ ? immune : infection, in);
		}
		// perform attacks in decreasing order of initiative...
		for(auto a: init_order)
		{
			auto& att = in[a];
			if(att.tgt_ != -1 && att.units_ > 0)
			{
				auto& def = in[att.tgt_];
				auto damage = calculate_damage(att, def);
				auto units = damage / def.hit_points_;
				def.units_ -= units;
				if(def.units_ < 0)
					def.units_ = 0;
//				fmt::println("{}({}) attacks {} with {} kills {}", a, att.initiative_, att.tgt_, damage, units);
			}
		}
		++rnd;
//		dump(in, rnd);
	}
	dump(in, rnd);
	return std::ranges::fold_left(in, 0, [](auto s, auto& v)
		{
			return s + v.units_;
		});
}
// 12403 too high
//  8834 too low

// input 2 26343
//

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
