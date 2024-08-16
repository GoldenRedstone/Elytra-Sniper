CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
project(ESniper)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

# set(SFML_FOUND TRUE)
set(SFML_STATIC_LIBRARIES FALSE)
find_package(SFML 2.5.1 COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(ESniper PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

add_subdirectory(vendor/imgui-sfml)
target_link_libraries(ESniper
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

