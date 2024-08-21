#ifndef ES_FRONTEND_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <array>
#include "biomes.h"

namespace es{
    const sf::Color color_route { 15, 83, 255, 255 };
    const sf::Color color_r_border { 15, 38, 245, 255 };

    const sf::Color color_void { 26, 26, 51 };
    const sf::Color color_barr { 161, 160, 152 };
    const sf::Color color_midl { 241, 247, 213 };
    const sf::Color color_high { 220, 215, 160 };
    using colorMap_t = std::shared_ptr<std::array<std::array<sf::Color, 150>, 150>>;

    colorMap_t generate_ColorMap (const MCVersion& mc, uint64_t& seed, const int64_t& sx, const int64_t& sz, const int mapScale);
    std::shared_ptr<sf::RenderTexture> generate_map (const sf::Window& win, const es::colorMap_t& CM);
    void ImGuiTheme();
    sf::RectangleShape createThickLine(const sf::Vector2f& start, const sf::Vector2f& end, float thickness, const sf::Color& color);
    void drawPath (sf::RenderWindow& win, const sf::Vector2f& start, const sf::Vector2f& end);

}

#endif // ES_FRONTEND_HPP
