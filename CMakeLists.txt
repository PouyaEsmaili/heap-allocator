cmake_minimum_required(VERSION 3.24)
project(heap_allocator C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(heap_allocator
        main.c
        mem.c
        mem.h)
