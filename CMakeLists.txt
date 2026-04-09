cmake_minimum_required(VERSION 3.20)
project(ohlcv_validator VERSION 1.0 LANGUAGES CXX)

# We want modern C++ features (C++20 adds concepts and better threading/modules)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable maximum warnings so we catch any sloppy C++ code immediately
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# We'll just define the include directory for now.
# Later, we will use CMake's FetchContent to pull down simdjson.
include_directories(src)

# We will add an executable here later once we write main.cpp
