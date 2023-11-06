cmake_minimum_required(VERSION 3.0.0)
project(AsyncServerLib CXX)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_CXX_STANDARD 20)

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

add_subdirectory(lib/Connection)
# add_subdirectory(lib/ServerListener)

add_library(ServerListenerLib
    include/ServerListener/ServerListener.h
)

add_library(AsyncServerLib STATIC
    include/AsyncServer/AsyncServer.h
    src/AsyncServer.cpp 
    # include/ServerListener/ServerListener.h
    # src/ServerListener.cpp 
)

target_link_libraries(ServerListenerLib
    PRIVATE AsyncServerLib
)

target_link_libraries(AsyncServerLib
    PUBLIC ConnectionLib
    PUBLIC ServerListenerLib
)

target_include_directories(ServerListenerLib
    PUBLIC include
)

target_include_directories(AsyncServerLib
    PUBLIC include
)
