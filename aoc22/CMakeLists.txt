cmake_minimum_required (VERSION 3.21)

add_executable(aoc22 aoc22.cpp)
target_link_libraries(aoc22 PRIVATE ctre::ctre fmt::fmt)