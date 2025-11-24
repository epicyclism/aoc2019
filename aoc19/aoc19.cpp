#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>

#include <fmt/format.h>

#include "timer.h"

int64_t execute(int64_t r0)
{
	timer t("execute");
    int64_t R0, R1, R2, R3, R4, R5, IP;
//#ip 1
    R0 = r0, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0;
l00: goto l17;//addi 1 16 1
l01:
#if 0
     R5 = 1;  //seti 1 2 5
l02: R2 = 1;//seti 1 2 2
l03: R3 = R5 * R2;//mulr 5 2 3
l04: R3 = R3 == R4; //eqrr 3 4 3
l05: if(R3) goto l07;//addr 3 1 1
l06: goto l08;//addi 1 1 1
l07: R0 = R0 + R5;//addr 5 0 0
l08: R2 = R2 + 1;//addi 2 1 2
l09: R3 = R2 > R4;//gtrr 2 4 3
l10: if(R3) goto l12;//addr 1 3 1
l11: goto l03;//seti 2 8 1
l12: ++R5;//addi 5 1 5
l13: R3 = R5 > R4;//gtrr 5 4 3
l14: if(R3) goto l16;//addr 3 1 1
l15: goto l02;//seti 1 1 1
#else
    for(R5 = 1; R5 <= R4; ++R5)
        if(R4 % R5 == 0)
            R0 += R5;
#endif
l16: goto lxx;//mulr 1 1 1
l17: R4 += 2;//addi 4 2 4
l18: R4 *= R4;//mulr 4 4 4
l19: R4 = R4 * 19;//mulr 1 4 4
l20: R4 = R4 * 11;//muli 4 11 4
l21: R3 = R3 + 3;//addi 3 3 3
l22: R3 = R3 * 22;//mulr 3 1 3
l23: R3 = R3 + 4;//addi 3 4 3
l24: R4 = R4 + R3;//addr 4 3 4
l25: if(R0) goto l27;//addr 1 0 1
l26: goto l01;//seti 0 0 1
l27: R3 = 27;//setr 1 5 3
l28: R3 = R3 * 28;//mulr 3 1 3
l29: R3 = 29 + R3;//addr 1 3 3
l30: R3 = 30 * R3;//mulr 1 3 3
l31: R3 = R3 * 14;//muli 3 14 3
l32: R3 = R3 * 32;//mulr 3 1 3
l33: R4 = R4 + R3;//addr 4 3 4
l34: R0 = 0;//seti 0 0 0
l35: goto l01;//seti 0 1 1
lxx:
	return R0;
}

// 10551306 too low

int main()
{
	auto p1 = execute(0);
	auto p2 = execute(1);
	fmt::println("pt1 = {}", p1);
	fmt::println("pt2 = {}", p2);
}
