cmake_minimum_required(VERSION 3.0.0)
project(ServerTests CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(Boost 1.74.0 REQUIRED system)
find_package(Threads REQUIRED)

add_subdirectory(TestClient)

add_executable( ServerTests
    ConnectionTests.cpp
    ServerTests.cpp
)

target_link_libraries(
    ServerTests
    PUBLIC GTest::gtest_main
    PRIVATE testClinet
    PRIVATE AsyncServerLib
    PRIVATE Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(ServerTests)