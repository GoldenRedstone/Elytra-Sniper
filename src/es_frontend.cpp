#include "es_frontend.hpp"
#include "generator.h"
#include <imgui.h>

namespace es 
{

colorMap_t generate_ColorMap (const MCVersion& mc, uint64_t& seed, const int64_t& sx, const int64_t& sz)
{
    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, DIM_END, seed);
    
    colorMap_t CM { std::make_shared<std::array<std::array<sf::Color, 150>, 150>>() };
    for (uint64_t x = 0; x < CM->size(); x++)
    {
        for (uint64_t z = 0; z < CM->at(x).size(); z++) 
        {
            int biomeID = getBiomeAt(&g, 1, sx+(x*16), 63, sz+(z*16)); // scale, x, y, z 
            if (biomeID == small_end_islands)    
                CM->at(x).at(z) = color_void;
            else if (biomeID == end_barrens)
                CM->at(x).at(z) = color_barr;
            else if (biomeID == end_midlands)
                CM->at(x).at(z) = color_midl;
            else if (biomeID == end_highlands)
                CM->at(x).at(z) = color_high;
        }
    }
    return CM;
}

std::shared_ptr<sf::RenderTexture> generate_map (const sf::Window& win, const colorMap_t& CM)
{
    std::shared_ptr<sf::RenderTexture> map { std::make_shared<sf::RenderTexture>() };
    map->create(win.getSize().x, win.getSize().y);
    map->clear();
    for (uint64_t x = 0; x < CM->size(); x++)
    {
        for (uint64_t z = 0; z < CM->at(x).size(); z++) 
        {
            sf::RectangleShape rect {{4.f, 4.f}};
            rect.setPosition({static_cast<float>(x * 4), static_cast<float>(win.getSize().y - z * 4)});
            rect.setFillColor(CM->at(x).at(z));
            map->draw(rect);
        }
    }
    return map;
}

void ImGuiTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    // Background colors
    colors[ImGuiCol_WindowBg]        = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White background
    colors[ImGuiCol_ChildBg]         = ImVec4(1.0f, 1.0f, 1.0f, 0.0f); // Transparent child background
    colors[ImGuiCol_PopupBg]         = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White popup background
    colors[ImGuiCol_Border]          = ImVec4(0.7f, 0.7f, 0.7f, 1.0f); // Light gray border
    // Text colors
    colors[ImGuiCol_Text]            = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black text
    colors[ImGuiCol_TextDisabled]    = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray text for disabled items
    // Header colors
    colors[ImGuiCol_Header]          = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Light gray header
    colors[ImGuiCol_HeaderHovered]   = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // Slightly lighter gray on hover
    colors[ImGuiCol_HeaderActive]    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White header active
    // Button colors
    colors[ImGuiCol_Button]           = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Light gray button
    colors[ImGuiCol_ButtonHovered]    = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // Slightly lighter gray on hover
    colors[ImGuiCol_ButtonActive]     = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White button active
    // Frame colors
    colors[ImGuiCol_FrameBg]          = ImVec4(0.8f, 0.8f, 0.8f, 0.8f); // White frame background
    colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // Slightly darker gray on hover
    colors[ImGuiCol_FrameBgActive]    = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); // Light gray active frame background
    // Tab colors
    colors[ImGuiCol_Tab]              = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // Light gray tab
    colors[ImGuiCol_TabHovered]       = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White tab on hover
    colors[ImGuiCol_TabActive]        = ImVec4(0.7f, 0.7f, 0.7f, 1.0f); // Gray tab active
    // Other colors
    colors[ImGuiCol_SliderGrab]       = ImVec4(0.7f, 0.7f, 0.7f, 1.0f); // Gray slider grab
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // Slightly lighter gray slider grab active
    // Optionally set other style properties
    style.FrameRounding = 10.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 10.0f;

    style.TabRounding = 8.f;
    style.FrameRounding = 8.f;
    style.GrabRounding = 8.f;
    style.WindowRounding = 8.f;
    style.PopupRounding = 8.f;
}

sf::RectangleShape createThickLine(const sf::Vector2f& start, const sf::Vector2f& end, float thickness, const sf::Color& color)
{
    sf::Vector2f direction = end - start;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length;

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setFillColor(color);

    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f; // Convert to degrees

    line.setOrigin({0.f, line.getSize().y / 2.f});
    line.setPosition(start);
    line.setRotation(angle);

    return line;
}

void drawPath (sf::RenderWindow& win, const sf::Vector2f& start, const sf::Vector2f& end)
{
    win.draw(es::createThickLine(
        start,
        end,
        14.f,
        { 89, 172, 255, 170 } 
    ));
    win.draw(es::createThickLine(
        start,
        end,
        8.f,
        { 8, 0, 188, 255 }
    ));
}

} // namespace es
