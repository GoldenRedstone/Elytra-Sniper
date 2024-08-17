#ifndef ES_FRONTEND_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <array>

namespace es{

    const sf::Color color_void { 0, 0, 42 };
    const sf::Color color_barr { 90, 90, 72 };
    const sf::Color color_midl { 235, 248, 182 };
    const sf::Color color_high { 195, 189, 89 };
    using colorMap_t = std::shared_ptr<std::array<std::array<sf::Color, 150>, 150>>;

    colorMap_t generate_ColorMap ();
    std::shared_ptr<sf::RenderTexture> generate_map (const sf::Window& win, const es::colorMap_t& CM);

}

#endif // ES_FRONTEND_HPP
