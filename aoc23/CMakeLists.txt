cmake_minimum_required (VERSION 3.21)

add_executable(aoc23 aoc23.cpp)
target_link_libraries(aoc23 PRIVATE ctre::ctre fmt::fmt)