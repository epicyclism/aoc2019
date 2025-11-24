cmake_minimum_required (VERSION 3.21)

add_executable(aoc21 aoc21.cpp)
target_link_libraries(aoc21 PRIVATE ctre::ctre fmt::fmt)