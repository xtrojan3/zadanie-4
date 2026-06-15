cmake_minimum_required(VERSION 3.26)
project(z4 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON) # pri kompilacii sa povolia aj rozsirenia GNU - kvoli getopt()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(z4 src/z4.c src/monopoly.c)
target_include_directories(z4 PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(z4 m)
