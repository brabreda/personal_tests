cmake_minimum_required(VERSION 3.22)
project(vectorized_copy LANGUAGES CXX)

find_package(ZLIB REQUIRED)
find_package(benchmark REQUIRED)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Specify the source files
set(SOURCES
  src/benchmark.cpp
)
set(PROJECT_NAME vectorized_copy)


# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# If you need to link any libraries, you can specify them here
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} benchmark::benchmark)

# add avx2 intrinsics
target_compile_options(vectorized_copy PRIVATE -mavx2)

