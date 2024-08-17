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
        { -1208, 4248, 0, 0 },  // 0
        { 376, 5176, 0, 0 },    // 1
        { 408, 4520, 0, 0 },    // 2
        { 56, 4856, 1, 0 },     // 3
        { 40, 4216, 1, 0 },     // 4
        { -616, 4248, 1, 0 },   // 5
        { -1224, 4840, 0, 0 }   // 6
    };

    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(60);
    bool res = ImGui::SFML::Init(window); // function is marked [[NODISCARD]]
    if (!res){
        return 1;
    }

    MCVersion mc = MC_1_20;
    uint64_t seed = 2438515238773172647;
    uint64_t startX = -1800, startZ = 4000;
    uint64_t playerX = startX + 1200, playerZ = startZ + 1200;
    
    es::colorMap_t colorMap { es::generate_ColorMap(mc, seed, startX, startZ) }; 
    std::shared_ptr<sf::RenderTexture> map { es::generate_map(window, colorMap) };
    sf::Sprite mapSprite { map->getTexture() };
    sf::Texture city_icon;
    city_icon.loadFromFile("assets/city.png");
    sf::Texture ship_icon;
    ship_icon.loadFromFile("assets/ship.png");
    sf::Texture player_icon;
    player_icon.loadFromFile("assets/player.png");
    
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



        for (int i = 0; i < cities.size()-1; i++) {
            es::drawPath(
                window,
                { static_cast<float>(cities.at(i  ).x - startX) / 16.f*4.f, static_cast<float>(cities.at(i  ).z - startZ) / 16.f*4.f },
                { static_cast<float>(cities.at(i+1).x - startX) / 16.f*4.f, static_cast<float>(cities.at(i+1).z - startZ) / 16.f*4.f }
            );
        }

        es::drawPath(
            window,
            { static_cast<float>(playerX - startX) / 16.f*4.f, static_cast<float>(playerZ - startZ) / 16.f*4.f },
            { static_cast<float>(cities.at(0).x - startX) / 16.f*4.f, static_cast<float>(cities.at(0).z - startZ) / 16.f*4.f }
        );

        int scale = 10;
        int mx = (playerX - startX) / 16*4 - scale/2;
        int mz = (playerZ - startZ) / 16*4 - scale/2;
        sf::Sprite sprite;
        sprite.setTexture(player_icon);
        sprite.setScale(3, 3);
        sprite.setPosition(mx, mz);
        window.draw(sprite);
        for (const CityLocation& city : cities) {

            int mx = (city.x - startX) / 16*4 - scale/2;
            int mz = (city.z - startZ) / 16*4 - scale/2;

            sf::Sprite sprite;
            sprite.setTexture((city.hasShip) ? ship_icon : city_icon);
            sprite.setScale(3, 3);
            sprite.setPosition(mx, mz);
            window.draw(sprite);
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
        ImGui::Begin("User Input", nullptr, window_flags);
            ImGui::PushItemWidth(100);
                ImGui::InputScalar("Seed", ImGuiDataType_U64, &seed, nullptr, nullptr, "%lu");
                ImGui::InputScalar("X", ImGuiDataType_S64, &playerX, nullptr, nullptr, "%ld");
                ImGui::InputScalar("Z", ImGuiDataType_S64, &playerZ, nullptr, nullptr, "%ld");
                static bool buttonPressed = false;
                if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) buttonPressed = true;
            ImGui::PopItemWidth();
            
            if (ImGui::Button("Regenerate") || buttonPressed)
            {
                startX = playerX - 1200;
                startZ = playerZ - 1200;
                colorMap = es::generate_ColorMap(mc, seed, startX, startZ);
                map =  es::generate_map(window, colorMap);
                mapSprite.setTexture(map->getTexture());
                buttonPressed = false;
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
