cmake_minimum_required (VERSION 3.21)

add_executable(aoc10 aoc10.cpp)
target_link_libraries(aoc10 PRIVATE ctre::ctre fmt::fmt)