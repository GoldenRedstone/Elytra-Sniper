#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#ifndef ES_FRONTEND_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <array>
#include "biomes.h"

namespace es{

    const sf::Color color_void { 0, 0, 42 };
    const sf::Color color_barr { 90, 90, 72 };
    const sf::Color color_midl { 235, 248, 182 };
    const sf::Color color_high { 195, 189, 89 };
    using colorMap_t = std::shared_ptr<std::array<std::array<sf::Color, 150>, 150>>;

    colorMap_t generate_ColorMap (const MCVersion& mc, uint64_t& seed, const int64_t& sx, const int64_t& sz, const int mapScale);
    /*std::shared_ptr<sf::RenderTexture> generate_map (const sf::Window& win, const es::colorMap_t& CM);*/
    std::shared_ptr<sf::Texture> generate_map (const sf::Window& win, const es::colorMap_t& CM);
    void ImGuiTheme();
    sf::RectangleShape createThickLine(const sf::Vector2f& start, const sf::Vector2f& end, float thickness, const sf::Color& color);
    void drawPath (sf::RenderWindow& win, const sf::Vector2f& start, const sf::Vector2f& end);

}

#endif // ES_FRONTEND_HPP
