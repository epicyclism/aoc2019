cmake_minimum_required (VERSION 3.21)

add_executable(aoc13 aoc13.cpp)
target_link_libraries(aoc13 PRIVATE ctre::ctre fmt::fmt)