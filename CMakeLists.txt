cmake_minimum_required(VERSION 3.30.1)  #Specify the minimum CMake version required
project(math_evaluator C)  #Define the project name and specify that using C

set(CMAKE_C_STANDARD 11)  #Set C standard

add_executable(math_evaluator src/main.c src/lex.c)  #Add executable (source files are in src/)

target_include_directories(math_evaluator PRIVATE include)  # Include the header files from /include directory