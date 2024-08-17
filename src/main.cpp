#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "generator.h"
#include "finders.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include "cityparser.hpp"
#include "es_frontend.hpp"

int main() {
    std::vector<CityLocation> cities = {
        { -1208, 4248, 0, 0 },
        { 376, 5176, 0, 0 },
        { 408, 4520, 0, 0 },
        { 56, 4856, 1, 0 },
        { 40, 4216, 1, 0 },
        { -616, 4248, 1, 0 },
        { -1224, 4840, 0, 0 }
    };

    for (const CityLocation& city : cities) {
        std::cout << city.x << ", " << city.y << ", " << city.hasShip << ", " << city.looted;
        std::cout << "\n";
    }

    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(60);
    bool res = ImGui::SFML::Init(window); // function is marked [[NODISCARD]]
    if (!res){
        return 1;
    }

    MCVersion mc = MC_1_20;
    uint64_t seed = 1;
    int64_t startX = -1800, startZ = 4000;
    
    es::colorMap_t colorMap { es::generate_ColorMap(mc, seed, startX, startZ) }; 
    std::shared_ptr<sf::RenderTexture> map { es::generate_map(window, colorMap) };
    sf::Sprite mapSprite { map->getTexture() };
    sf::Texture city_icon;
    city_icon.loadFromFile("assets/city.png");
    sf::Texture ship_icon;
    ship_icon.loadFromFile("assets/ship.png");
    
    es::ImGuiTheme();
    sf::Clock deltaClock;
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
                goto shutdown; // I am not scared a litte goto from time to time!
            else if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::Escape)
                    goto shutdown;
        }

        window.clear();

        window.draw(mapSprite);



        for (const CityLocation& city : cities) {
            int scale = 10;

            int mx = (city.x - startX) / 16*4;
            int my = (city.y - startZ) / 16*4;

            sf::Sprite sprite;
            sprite.setTexture((city.hasShip) ? ship_icon : city_icon);
            sprite.setScale(3, 3);
            sprite.setPosition(mx, my);
            window.draw(sprite);
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
        ImGui::Begin("User Input", nullptr, window_flags);
            ImGui::PushItemWidth(100);
                ImGui::InputScalar("Seed", ImGuiDataType_U64, &seed, nullptr, nullptr, "%lu");
                ImGui::InputScalar("startX", ImGuiDataType_S64, &startX, nullptr, nullptr, "%ld");
                ImGui::InputScalar("startZ", ImGuiDataType_S64, &startZ, nullptr, nullptr, "%ld");
                static bool buttonPressed = false;
                if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) buttonPressed = true;
            ImGui::PopItemWidth();
            
            if (ImGui::Button("Regenerate") || buttonPressed)
            {
                colorMap = es::generate_ColorMap(mc, seed, startX, startZ); 
                map =  es::generate_map(window, colorMap);
                mapSprite.setTexture(map->getTexture());
                buttonPressed = false;
            }
        ImGui::End();
        ImGui::SFML::Render(window);

        window.draw(es::createThickLine(
            { 100.f, 100.f },
            { 500.f, 500.f },
            8.f,
            { 255, 0, 0, 255 }
        ));
        window.draw(es::createThickLine(
            { 100.f, 100.f },
            { 500.f, 500.f },
            4.f,
            { 0, 255, 0, 255 }
        ));
        
        window.display();
    }

shutdown:
    window.close();
    ImGui::SFML::Shutdown();

    return 0;
}
