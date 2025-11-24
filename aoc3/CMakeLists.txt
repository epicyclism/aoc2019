cmake_minimum_required (VERSION 3.21)

add_executable(aoc3 aoc3.cpp)
target_link_libraries(aoc3 PRIVATE ctre::ctre fmt::fmt)
