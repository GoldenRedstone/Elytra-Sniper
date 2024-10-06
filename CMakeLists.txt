CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
project(ESniper)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

# cubiomes
add_subdirectory(vendor/cubiomes)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/cubiomes)
target_link_libraries(ESniper PRIVATE cubiomes)
target_include_directories(ESniper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cubiomes)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwrapv -lm")

# SFML
set(SFML_STATIC_LIBRARIES FALSE)
find_package(SFML 2.5.1 COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(ESniper PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# ImGui
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
# target_include_directories(ESniper PRIVATE ${IMGUI_DIR})

# SFML-ImGui
add_subdirectory(vendor/imgui-sfml)
target_link_libraries(ESniper
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

# OpenMP
find_package(OpenMP REQUIRED)  # Add this line to find OpenMP

if(OpenMP_CXX_FOUND)  # Check if OpenMP was found
    target_link_libraries(ESniper PRIVATE OpenMP::OpenMP_CXX)  # Link OpenMP
endif()

file(MAKE_DIRECTORY searched)
