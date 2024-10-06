#include "es_frontend.hpp"
#include "generator.h"
#include <imgui.h>
#include <omp.h>

namespace es 
{

colorMap_t generate_ColorMap (const MCVersion& mc, uint64_t& seed, const int64_t& sx, const int64_t& sz, const int mapScale)
{
    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, DIM_END, seed);
    
    colorMap_t CM { std::make_shared<std::array<std::array<sf::Color, 150>, 150>>() };
    #pragma omp parallel for collapse(2)
    for (uint64_t x = 0; x < CM->size(); x++) {
        for (uint64_t z = 0; z < CM->at(0).size(); z++) {
            int biomeID = getBiomeAt(&g, 1, sx+(x*mapScale), 63, sz+(z*mapScale)); // scale, x, y, z 
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

std::shared_ptr<sf::RenderTexture> generate_map(const sf::Window& win, const colorMap_t& CM) {
    auto map = std::make_shared<sf::RenderTexture>();
    if (!map->create(win.getSize().x, win.getSize().y)) {
        throw std::runtime_error("Failed to create render texture.");
    }
    map->clear();

    std::vector<sf::Vertex> vertices; // Use a vertex array for batching
    vertices.reserve(150 * 150 * 4); // Reserve enough space for all rectangles

    for (size_t x = 0; x < CM->size(); x++) {
        for (size_t z = 0; z < CM->at(x).size(); z++) {
            sf::Color fillColor = CM->at(x).at(z);
            float xPos = static_cast<float>(x * 4);
            float yPos = static_cast<float>(win.getSize().y - z * 4);

            // Create a quad for each rectangle
            vertices.emplace_back(sf::Vector2f(xPos, yPos), fillColor);
            vertices.emplace_back(sf::Vector2f(xPos + 4.f, yPos), fillColor);
            vertices.emplace_back(sf::Vector2f(xPos + 4.f, yPos + 4.f), fillColor);
            vertices.emplace_back(sf::Vector2f(xPos, yPos + 4.f), fillColor);
        }
    }

    // Draw all vertices in one call
    map->draw(&vertices[0], vertices.size(), sf::Quads);
    
    return map;
}

void ImGuiTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Setting colors using a loop for clarity and maintainability
    std::array<std::pair<ImGuiCol, ImVec4>, 18> colorSettings = {{
        {ImGuiCol_WindowBg,        ImVec4(1.0f, 1.0f, 1.0f, 1.0f)},
        {ImGuiCol_ChildBg,         ImVec4(1.0f, 1.0f, 1.0f, 0.0f)},
        {ImGuiCol_PopupBg,         ImVec4(1.0f, 1.0f, 1.0f, 1.0f)},
        {ImGuiCol_Border,          ImVec4(0.7f, 0.7f, 0.7f, 1.0f)},
        {ImGuiCol_Text,            ImVec4(0.0f, 0.0f, 0.0f, 1.0f)},
        {ImGuiCol_TextDisabled,    ImVec4(0.5f, 0.5f, 0.5f, 1.0f)},
        {ImGuiCol_Header,          ImVec4(0.8f, 0.8f, 0.8f, 1.0f)},
        {ImGuiCol_HeaderHovered,   ImVec4(0.9f, 0.9f, 0.9f, 1.0f)},
        {ImGuiCol_HeaderActive,    ImVec4(1.0f, 1.0f, 1.0f, 1.0f)},
        {ImGuiCol_Button,          ImVec4(0.8f, 0.8f, 0.8f, 1.0f)},
        {ImGuiCol_ButtonHovered,   ImVec4(0.9f, 0.9f, 0.9f, 1.0f)},
        {ImGuiCol_ButtonActive,    ImVec4(1.0f, 1.0f, 1.0f, 1.0f)},
        {ImGuiCol_FrameBg,         ImVec4(0.8f, 0.8f, 0.8f, 0.8f)},
        {ImGuiCol_FrameBgHovered,  ImVec4(0.9f, 0.9f, 0.9f, 1.0f)},
        {ImGuiCol_FrameBgActive,   ImVec4(0.8f, 0.8f, 0.8f, 1.0f)},
        {ImGuiCol_Tab,             ImVec4(0.9f, 0.9f, 0.9f, 1.0f)},
        {ImGuiCol_TabHovered,      ImVec4(1.0f, 1.0f, 1.0f, 1.0f)},
        {ImGuiCol_TabActive,       ImVec4(0.7f, 0.7f, 0.7f, 1.0f)}
    }};

    // Apply colors
    for (const auto& colorSetting : colorSettings) {
        colors[colorSetting.first] = colorSetting.second;
    }

    style.FrameRounding = 10.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 10.0f;
    style.TabRounding = 8.f;
    style.FrameRounding = 8.f;
    style.GrabRounding = 8.f;
    style.WindowRounding = 8.f;
    style.PopupRounding = 8.f;
}

sf::RectangleShape createThickLine(const sf::Vector2f& start, const sf::Vector2f& end, float thickness, const sf::Color& color) {
    sf::Vector2f direction = end - start;
    float length = std::sqrt(direction.x*direction.x + direction.y*direction.y); // Use hypot for better precision
    if (length == 0) return sf::RectangleShape(); // Return an empty shape if length is zero
    direction /= length;

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setFillColor(color);

    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f; // Convert to degrees
    line.setOrigin(0.f, thickness / 2.f); // Set origin to the center vertically
    line.setPosition(start);
    line.setRotation(angle);

    return line;
}

void drawPath(sf::RenderWindow& win, const sf::Vector2f& start, const sf::Vector2f& end) {
    win.draw(es::createThickLine(start, end, 12.f, color_r_border));
    win.draw(es::createThickLine(start, end, 8.f, color_route));
}

} // namespace es
