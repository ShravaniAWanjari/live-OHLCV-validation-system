cmake_minimum_required(VERSION 3.15)
project(ohlcv_Validator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OPENSSL_ROOT_DIR "C:/msys64/mingw64")

find_package(OpenSSL REQUIRED)

include(FetchContent)

FetchContent_Declare(
    simdjson
    GIT_REPOSITORY
    https://github.com/simdjson/simdjson.git
    GIT_TAG v3.10.1
)

FetchContent_MakeAvailable(simdjson)

set(USE_TLS ON CACHE BOOL "" FORCE)
set(USE_OPEN_SSL ON CACHE BOOL "" FORCE)
set(USE_ZLIB OFF CACHE BOOL "" FORCE) 
set(USE_WS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    ixwebsocket
    GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
    GIT TAG v11.4.4
)

FetchContent_MakeAvailable(ixwebsocket)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native")

add_executable(validator src/main.cpp)
target_link_libraries(validator PRIVATE 
    simdjson::simdjson
    ixwebsocket::ixwebsocket
    OpenSSL::SSL
    OpenSSL::Crypto
)


