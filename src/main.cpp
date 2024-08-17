#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "generator.h"
#include "finders.h"
#include <memory>
#include <iostream>
#include <array>

#include "es_frontend.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(60);
    bool res = ImGui::SFML::Init(window); // function is marked [[NODISCARD]]
    if (!res){
        return 1;
    }

    uint64_t seed = 1;
    int64_t startX = -1800, startZ = 4000;
    es::colorMap_t colorMap { es::generate_ColorMap(seed, startX, startZ) }; 
    std::shared_ptr<sf::RenderTexture> map { es::generate_map(window, colorMap) };
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
            if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::Escape)
                    goto shutdown;
        }

        window.clear();

        window.draw(mapSprite);

        ImGui::SFML::Update(window, deltaClock.restart());
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
        ImGui::Begin("User Input", nullptr, window_flags);
            ImGui::PushItemWidth(100);
            ImGui::InputScalar("Seed", ImGuiDataType_U64, &seed, nullptr, nullptr, "%lu");
            ImGui::InputScalar("startX", ImGuiDataType_S64, &startX, nullptr, nullptr, "%ld");
            ImGui::InputScalar("startZ", ImGuiDataType_S64, &startZ, nullptr, nullptr, "%ld");
            ImGui::PopItemWidth();
            if (ImGui::Button("Regenerate"))
            {
                colorMap = es::generate_ColorMap(seed, startX, startZ); 
                map =  es::generate_map(window, colorMap);
                mapSprite.setTexture(map->getTexture());
            }
        ImGui::End();
        
        ImGui::SFML::Render(window);
        
        window.display();
    }

shutdown:
    window.close();
    ImGui::SFML::Shutdown();

    return 0;
}
