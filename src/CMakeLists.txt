cmake_minimum_required(VERSION 3.0)
project("Compiler_Project")

add_subdirectory("chapter_2")
add_subdirectory("chapter_3")

add_executable("little_compiler" "little_compiler.cpp")

