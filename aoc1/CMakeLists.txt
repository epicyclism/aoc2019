cmake_minimum_required (VERSION 3.21)

add_executable(aoc1 aoc1.cpp)
target_link_libraries(aoc1 PRIVATE ctre::ctre fmt::fmt)