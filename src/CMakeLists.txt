include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(IMGUI_SOURCES
    ../vendor/imgui/imgui.cpp
    ../vendor/imgui/imgui_demo.cpp
    ../vendor/imgui/imgui_draw.cpp
    ../vendor/imgui/imgui_widgets.cpp
)

file(GLOB_RECURSE SRC *.cpp)
file(GLOB_RECURSE HEADER *.hpp)
add_executable(ESniper ${SRC} ${HEADER} ${IMGUI_SOURCES})

install(TARGETS ESniper DESTINATION out)

