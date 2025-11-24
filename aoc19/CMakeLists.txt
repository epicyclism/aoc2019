cmake_minimum_required (VERSION 3.21)

add_executable(aoc19 aoc19.cpp)
target_link_libraries(aoc19 PRIVATE ctre::ctre fmt::fmt)

add_executable(aoc19_1 aoc19_1.cpp)
target_link_libraries(aoc19_1 PRIVATE ctre::ctre fmt::fmt)