cmake_minimum_required (VERSION 3.21)

add_executable(aoc17 aoc17.cpp)
target_link_libraries(aoc17 PRIVATE ctre::ctre fmt::fmt)