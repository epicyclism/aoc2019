#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "ctre_inc.h"
#include "timer.h"

constexpr unsigned radiation   = 0x00000001;
constexpr unsigned bludgeoning = 0x00000002;
constexpr unsigned slashing    = 0x00000004;
constexpr unsigned fire        = 0x00000008;
constexpr unsigned cold        = 0x00000010;

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
	group_t* tgt_ = nullptr;
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
	g.tgt_ = nullptr;
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
	if(auto[m, w] = ctre::search<"weak to ([^;\\)]+)">(ln); m)
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

int calculate_damage(group_t const& attacker, group_t const& defender)
{
	if(attacker.attacktype_ & defender.immunities_)
		return 0;
	int base = effective_damage(attacker);
	if( attacker.attacktype_ & defender.weaknesses_)
		base *= 2;

	return base;
}

void choose_target(group_t& attacker, std::vector<group_t*>& tgts)
{
	// sort tgts by damage from this attacker.
	std::ranges::sort(tgts, [&](auto l, auto r)
		{
			auto dl = calculate_damage(attacker, *l);
			auto dr = calculate_damage(attacker, *r);
			if(dl == dr)
			{
				auto epl = effective_damage(*l);
				auto epr = effective_damage(*r);
				if(epl == epr)
					return l->initiative_ > r->initiative_;
				return epl > epr;
			}
			return dl > dr; 
		});
	for(auto t: tgts)
	{
		if(!t->tgtd_ && calculate_damage(attacker, *t))
		{
			attacker.tgt_ = t;
			t->tgtd_ = true;
			return;
		}
	}

}

std::pair<int, int> fight(auto in, int up = 0)
{
	for(auto& g: in)
	{
		if( !g.infection_)
			g.attack_damage_ += up;
	}
	std::vector<group_t*> attack_order;
	std::vector<group_t*> init_order;
	attack_order.resize(in.size());
	init_order.resize(in.size());
	std::ranges::transform(in, std::ranges::begin(init_order), [](auto& g) { return &g;});
	std::ranges::sort(init_order, [&](auto l, auto r)
			{
				return l->initiative_ > r->initiative_;
			});
	std::vector<group_t*> infection;
	std::vector<group_t*> immune;
	int rnds = 0;

	while(1)
	{
		reset(in);
		// find the groups that can attack
		attack_order.resize(0);
		immune.resize(0);
		infection.resize(0);
		for(auto& g: in)
		{
			if(g.units_ > 0)
			{
				attack_order.push_back(&g);
				if( g.infection_)
					infection.push_back(&g);
				else
					immune.push_back(&g);
			}
		}
		if(infection.empty() || immune.empty())
			break;
		// sort the attack order
		std::ranges::sort(attack_order, [](auto l, auto r)
			{
				auto el = effective_damage(*l); auto er = effective_damage(*r);
				if(el == er)
					return l->initiative_ > r->initiative_;
				return el > er;
			});
		// for each attacker select a target
		for(auto g: attack_order)
			choose_target(*g, g->infection_ ? immune : infection);
		// perform attacks in decreasing order of initiative...
		for(auto a: init_order)
		{
			if(a->tgt_ != nullptr /*&& a->units_ > 0*/)
			{

				auto damage = calculate_damage(*a, *(a->tgt_));
				auto units = damage / a->tgt_->hit_points_;
				a->tgt_->units_ -= units;
				if(a->tgt_->units_ < 0)
					a->tgt_->units_ = 0;
			}
		}
		++rnds;
		if(rnds > 2500)
			return {0, 1};
	}
	return std::make_pair(
		std::ranges::fold_left(in, 0, [](auto s, auto& v)
		{
			return v.infection_ ? s : s + v.units_;
		}),
		std::ranges::fold_left(in, 0, [](auto s, auto& v)
		{
			return v.infection_ ? s + v.units_ : s;
		})
	);
}

int pt1(auto in)
{
	timer t("p1");
	auto[i1, i2] = fight(in);
	return i1 + i2;
}

int pt2(auto in)
{
	timer t("p2");
	int upH = 1000;
	int upL = 1;
	while(1)
	{
		auto[i1, _] = fight(in, upH);
		if(i1)
			break;
		upH += 10000;
	}
	while(upH - upL > 1)
	{
		int up = upL + (upH - upL) / 2;
		auto[i1, _] = fight(in, up);
		if(i1)
			upH = up;
		else
			upL = up;
	}
	auto[i1, _] = fight(in, upH);
	return i1;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
