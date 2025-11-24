cmake_minimum_required (VERSION 3.21)

add_executable(aoc14 aoc14.cpp)
target_link_libraries(aoc14 PRIVATE ctre::ctre fmt::fmt)