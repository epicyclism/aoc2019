#include <fmt/format.h>
#include "intcode.h"
#include "timer.h"

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto wkr(in);
	wkr.mem_[1] = 12;
	wkr.mem_[2] = 02;
	wkr.execute();
	return wkr.mem_[0];
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	for(int noun = 0; noun < 100; ++noun)
		for(int verb = 0; verb < 100; ++verb)
		{
			auto wkr (in);
			wkr.mem_[1] = noun;
			wkr.mem_[2] = verb;
			wkr.execute();
			if(wkr.mem_[0] == 19690720)
				return 100 * noun + verb;
		}
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
