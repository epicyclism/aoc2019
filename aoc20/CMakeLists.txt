cmake_minimum_required (VERSION 3.21)

add_executable(aoc20 aoc20.cpp)
target_link_libraries(aoc20 PRIVATE ctre::ctre fmt::fmt)