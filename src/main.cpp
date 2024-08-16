#include <SFML/Graphics.hpp>
#include "generator.h"
#include "finders.h"
#include <cstdlib>
#include <memory>
#include <iostream>
#include <vector>
#include <inttypes.h>

namespace ES{
    const sf::Color color_void { 0, 0, 42 };
    const sf::Color color_barr { 90, 90, 72 };
    const sf::Color color_midl { 235, 248, 182 };
    const sf::Color color_high { 195, 189, 89 };
}

int main()
{
    Generator g;
    SurfaceNoise sn;
    uint64_t seed = 1;
    setupGenerator(&g, MC_1_20, 0);
    applySeed(&g, DIM_END, seed);
    // initSurfaceNoise(&sn, DIM_END, seed);

    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(1);

    std::vector<std::vector<sf::Color>> colorMap{}; 
    for (uint64_t x = 0; x < 150; x++)
    {
        colorMap.emplace_back();
        for (uint64_t z = 0; z < 150; z++) 
        {
            int biomeID = getBiomeAt(&g, 1, 1000+(x*8), 63, 1000+(z*8)); // scale, x, y, z 
            if (biomeID == small_end_islands)    
                colorMap[x].push_back(ES::color_void);
            else if (biomeID == end_barrens)
                colorMap[x].push_back(ES::color_barr);
            else if (biomeID == end_midlands)
                colorMap[x].push_back(ES::color_midl);
            else if (biomeID == end_highlands)
                colorMap[x].push_back(ES::color_high);
        }
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
                sf::RectangleShape rect {{8.f, 8.f}};
                rect.setPosition({static_cast<float>(x * 8), static_cast<float>(z * 8)});
                rect.setFillColor(colorMap[x][z]);
                window.draw(rect);
            }
        }

        window.display();
    }

    return 0;
}
/*#include "imgui.h"*/
/*#include "imgui-SFML.h"*/
/**/
/*#include <SFML/Graphics/CircleShape.hpp>*/
/*#include <SFML/Graphics/RenderWindow.hpp>*/
/*#include <SFML/System/Clock.hpp>*/
/*#include <SFML/Window/Event.hpp>*/
/**/
/*int main() {*/
/*    sf::RenderWindow window(sf::VideoMode(640, 480), "ImGui + SFML = <3");*/
/*    window.setFramerateLimit(60);*/
/*    ImGui::SFML::Init(window);*/
/**/
/*    sf::CircleShape shape(100.f);*/
/*    shape.setFillColor(sf::Color::Green);*/
/**/
/*    sf::Clock deltaClock;*/
/*    while (window.isOpen()) {*/
/*        sf::Event event;*/
/*        while (window.pollEvent(event)) {*/
/*            ImGui::SFML::ProcessEvent(window, event);*/
/**/
/*            if (event.type == sf::Event::Closed) {*/
/*                window.close();*/
/*            }*/
/*        }*/
/**/
/*        ImGui::SFML::Update(window, deltaClock.restart());*/
/**/
/*        ImGui::ShowDemoWindow();*/
/**/
/*        ImGui::Begin("Hello, world!");*/
/*        ImGui::Button("Look at this pretty button");*/
/*        ImGui::End();*/
/**/
/*        window.clear();*/
/*        window.draw(shape);*/
/*        ImGui::SFML::Render(window);*/
/*        window.display();*/
/*    }*/
/**/
/*    ImGui::SFML::Shutdown();*/
/*}*/
