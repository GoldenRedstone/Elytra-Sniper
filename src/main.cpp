#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "generator.h"
#include "finders.h"
#include <memory>
#include <iostream>
#include <array>

namespace ES{
    const sf::Color color_void { 0, 0, 42 };
    const sf::Color color_barr { 90, 90, 72 };
    const sf::Color color_midl { 235, 248, 182 };
    const sf::Color color_high { 195, 189, 89 };
    using colorMap_t = std::shared_ptr<std::array<std::array<sf::Color, 150>, 150>>;

    ES::colorMap_t generate_ColorMap ()
    {
        Generator g;
        uint64_t seed = 1;
        setupGenerator(&g, MC_1_20, 0);
        applySeed(&g, DIM_END, seed);
        
        ES::colorMap_t CM { std::make_shared<std::array<std::array<sf::Color, 150>, 150>>() };
        for (uint64_t x = 0; x < CM->size(); x++)
        {
            for (uint64_t z = 0; z < CM->at(x).size(); z++) 
            {
                int biomeID = getBiomeAt(&g, 1, -1800+(x*8), 63, 4000+(z*8)); // scale, x, y, z 
                if (biomeID == small_end_islands)    
                    CM->at(x).at(z) = ES::color_void;
                else if (biomeID == end_barrens)
                    CM->at(x).at(z) = ES::color_barr;
                else if (biomeID == end_midlands)
                    CM->at(x).at(z) = ES::color_midl;
                else if (biomeID == end_highlands)
                    CM->at(x).at(z) = ES::color_high;
            }
        }
        return CM;
    }

    std::shared_ptr<sf::RenderTexture> generate_map (const sf::Window& win, const ES::colorMap_t& CM)
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
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(60);
    bool res = ImGui::SFML::Init(window); // function is marked [[NODISCARD]]
    if (!res){
        return 1;
    }

    ES::colorMap_t colorMap { ES::generate_ColorMap() }; 
    std::shared_ptr<sf::RenderTexture> map { ES::generate_map(window, colorMap) };
    sf::Sprite mapSprite { map->getTexture() };
    
    sf::Clock deltaClock;
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
                goto shutdown; // I am not scared a litte goto from time to time!
        }

        window.clear();

        window.draw(mapSprite);

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::ShowDemoWindow();
        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();
        ImGui::SFML::Render(window);
        
        window.display();
    }

shutdown:
    window.close();
    ImGui::SFML::Shutdown();

    return 0;
}
