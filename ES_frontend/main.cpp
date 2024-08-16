#include <SFML/Graphics.hpp>
#include "generator.h"
#include <iostream>
#include <vector>

int main()
{
    Generator g;
    uint64_t seed = 1;
    setupGenerator(&g, MC_1_20, 0);
    applySeed(&g, DIM_OVERWORLD, seed);

    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(1);

    std::vector<std::vector<sf::Color>> colorMap{};    

    for (uint64_t x = 0; x < 150; x++)
    {
        colorMap.emplace_back();
        for (uint64_t z = 0; z < 150; z++) 
        {
            int biomeID = getBiomeAt(&g, 1, x*4, 63, z*4); // scale, x, y, z 
            if (
                biomeID == river || biomeID == ocean || biomeID == frozen_ocean ||
                biomeID == deep_ocean || biomeID == warm_ocean || biomeID == lukewarm_ocean ||
                biomeID == cold_ocean  || biomeID == deep_warm_ocean ||biomeID == deep_lukewarm_ocean ||
                biomeID == deep_cold_ocean || biomeID == frozen_river
            )
                colorMap[x].push_back(sf::Color::Blue);
            else
                colorMap[x].push_back(sf::Color::Green);
        }
    }
    std::cout << "Finished Generating Colormap!" << std::endl;
    std::cout << "Colormap:" << std::endl;
    for (uint64_t x = 0; x < 150; x++)
    {
        for (uint64_t z = 0; z < 150; z++)    
        {
            std::cout << (colorMap[x][z] == sf::Color::Blue ? 1 : 0);
        }
        std::cout << std::endl;
    }

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (uint64_t x = 0; x < 150; x++)
        {
            for (uint64_t z = 0; z < 150; z++) 
            {
                sf::RectangleShape rect {{4.f, 4.f}};
                rect.setPosition({static_cast<float>(x * 4), static_cast<float>(z * 4)});
                rect.setFillColor(colorMap[x][z]);
                window.draw(rect);
            }
        }

        window.display();
    }

    return 0;
}
