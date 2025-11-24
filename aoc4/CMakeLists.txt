cmake_minimum_required (VERSION 3.21)

add_executable(aoc4 aoc4.cpp)
target_link_libraries(aoc4 PRIVATE ctre::ctre fmt::fmt)
