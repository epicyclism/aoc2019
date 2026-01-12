#include <fmt/format.h>


#include "intcode.h"
#include "timer.h"

int64_t pt1(auto const& in)
{
	timer t("p1");
	auto wkr(in);
	wkr.in_.push(1);
	wkr.execute();
	return wkr.buf_;
}

int64_t pt2(auto const& in)
{
	timer t("p2");
	auto wkr(in);
	wkr.in_.push(5);
	wkr.execute();
	return wkr.buf_;
}

int main()
{
	auto in = get_input();
	auto p1 = pt1(in);
	auto p2 = pt2(in);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
