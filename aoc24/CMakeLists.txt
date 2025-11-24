cmake_minimum_required (VERSION 3.21)

add_executable(aoc24 aoc24.cpp)
target_link_libraries(aoc24 PRIVATE ctre::ctre fmt::fmt)