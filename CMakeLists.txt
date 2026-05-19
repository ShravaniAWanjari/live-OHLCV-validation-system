cmake_minimum_required(VERSION 3.15)
project(ohlcv_Validator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    simdjson
    GIT_REPOSITORY
    https://github.com/simdjson/simdjson.git
    GIT_TAG v3.10.1
)

FetchContent_MakeAvailable(simdjson)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native")

add_executable(validator src/main.cpp)
target_link_libraries(validator PRIVATE simdjson::simdjson)


