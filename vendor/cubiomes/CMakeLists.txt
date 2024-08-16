CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
project(cubiomes)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC ./*.c)
file(GLOB_RECURSE HEADERS ./*.h)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# set(SRC 
#   ./*.c
# )

# set(HEADERS
#   ./*.h
# )

add_library(cubiomes SHARED ${SRC} ${HEADERS})


