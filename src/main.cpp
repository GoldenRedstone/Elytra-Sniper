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
    // std::vector<CityLocation> cities = parseCSVFile("searched/2438515238773172647.-5000.0.csv");
    std::vector<CityLocation> cities = readCitiesAround(2438515238773172647, -1200, 4600);

    // Print out our table
    for (const CityLocation& city : cities) {
        std::cout << city.x << ", " << city.z << ", " << city.hasShip << ", " << city.looted;
        std::cout << "\n";
    }

    printf("Hello World!\n");

    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(60);
    bool res = ImGui::SFML::Init(window); // function is marked [[NODISCARD]]
    if (!res){
        return 1;
    }

    MCVersion mc = MC_1_20;
    uint64_t seed = 2438515238773172647;
    int64_t startX = -1800, startZ = 4000;
    es::colorMap_t colorMap { es::generate_ColorMap(mc, seed, startX, startZ) }; 
    std::shared_ptr<sf::RenderTexture> map { es::generate_map(window, colorMap) };
    sf::Sprite mapSprite { map->getTexture() };
    
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

        sf::Texture city_icon;
        city_icon.loadFromFile(PROJECT_DIR("assets/city.png"));
        sf::Texture ship_icon;
        ship_icon.loadFromFile(PROJECT_DIR("assets/ship.png"));

        for (const CityLocation& city : cities) {

            int mx = (city.x - startX) / 16*4 - scale/2;
            int mz = (city.z - startZ) / 16*4 - scale/2;

            // sf::CircleShape circle;
            // circle.setRadius(scale);
            // circle.setFillColor((city.hasShip) ? sf::Color::Green : sf::Color::Red);
            // circle.setPosition(mx, mz);
            // window.draw(circle);

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
                ImGui::InputScalar("startX", ImGuiDataType_S64, &startX, nullptr, nullptr, "%ld");
                ImGui::InputScalar("startZ", ImGuiDataType_S64, &startZ, nullptr, nullptr, "%ld");
                static bool buttonPressed = false;
                if (ImGui::IsItemActive() || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) buttonPressed = true;
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
        
        window.display();
    }

shutdown:
    window.close();
    ImGui::SFML::Shutdown();

    return 0;
}
