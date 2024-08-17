include_directories(
  .
)

set(IMGUI_SOURCES
    ../vendor/imgui/imgui.cpp
    ../vendor/imgui/imgui_demo.cpp
    ../vendor/imgui/imgui_draw.cpp
    ../vendor/imgui/imgui_widgets.cpp
)

# file(GLOB_RECURSE SRC *.cpp)
file(GLOB_RECURSE HEADER *.hpp)
set(SRC 
  main.cpp
  cityparser.cpp
)

add_executable(ESniper ${SRC} ${HEADER} ${IMGUI_SOURCES})

target_compile_definitions(ESniper PRIVATE PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/")

install(TARGETS ESniper DESTINATION out)
