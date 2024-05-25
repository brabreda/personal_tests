cmake_minimum_required(VERSION 3.28)

# Set the project name and the language used
project(check_grade LANGUAGES CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Specify the source files
set(SOURCES
    main.cpp
)

# Add the executable target
add_executable(check_grade ${SOURCES})

# If you need to link any libraries, you can specify them here
target_link_libraries(check_grade)

# add avx2 intrinsics
target_compile_options(check_grade PRIVATE -mavx2)

