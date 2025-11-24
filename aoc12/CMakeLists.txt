cmake_minimum_required (VERSION 3.21)

add_executable(aoc12 aoc12.cpp)
target_link_libraries(aoc12 PRIVATE ctre::ctre fmt::fmt)