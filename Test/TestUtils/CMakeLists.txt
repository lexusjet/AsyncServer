cmake_minimum_required(VERSION 3.0.0)
project(TestUtils CXX)


set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_compile_options(
    
    # -Werror
    -Wall
    -Wno-comment
    -Wno-switch
    
    -Wextra
    -Wno-unused-parameter
    
    -Wpedantic
    -Wcast-align
    -Wcast-qual
    
    # -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    
    # -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    
    # -Wsign-conversion
    -Wsign-promo
    -Wno-unknown-pragmas
)

find_package(Boost 1.74.0 REQUIRED system)


add_library(TestUtilsLib STATIC
    include/TestClient/TestClient.cpp
    include/TestListner/TestListner.cpp
)

target_link_libraries(TestUtilsLib
    PRIVATE AsyncServerLib
)

target_include_directories(TestUtilsLib
    PUBLIC include
)