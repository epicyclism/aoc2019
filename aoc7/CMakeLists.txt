cmake_minimum_required (VERSION 3.21)

add_executable(aoc7 aoc7.cpp)
target_link_libraries(aoc7 PRIVATE ctre::ctre fmt::fmt)