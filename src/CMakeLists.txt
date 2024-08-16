add_executable(ESniper main.cpp)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
)

install(TARGETS ESniper DESTINATION out)




