cmake_minimum_required (VERSION 3.21)

add_executable(aoc9 aoc9.cpp)
target_link_libraries(aoc9 PRIVATE ctre::ctre fmt::fmt)