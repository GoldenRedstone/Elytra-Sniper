#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "generator.h"
#include "finders.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <limits>
#include "endcityfinder.hpp"
#include "cityparser.hpp"
#include "es_frontend.hpp"

int optimalScale(std::vector<CityLocation> cities, int64_t playerX, int64_t playerZ) {
    int maxDistance = 600;
    for (CityLocation city : cities) {
        if (abs(static_cast<int64_t>(city.x - playerX)) > maxDistance) {
            maxDistance = abs(static_cast<int64_t>(city.x - playerX));
        }
        if (abs(static_cast<int64_t>(city.z - playerZ)) > maxDistance) {
            maxDistance = abs(static_cast<int64_t>(city.z - playerZ));
        }
    }
    std::cout << maxDistance << "\n";
    return floor((maxDistance*1.1) / 75.f);
}

std::vector<CityLocation> GeneratePath (uint64_t visitCities, std::vector<CityLocation> cities, const int64_t& playerX, const int64_t& playerZ)
{
    std::vector<CityLocation> result;
    std::vector<CityLocation> tmp = cities;
    int64_t travellerX = playerX;
    int64_t travellerZ = playerZ;

    for (uint64_t i = 0; i < visitCities; i++)
    {
        CityLocation winner;
        double winningDist = std::numeric_limits<double>::max();
        uint64_t winningIndex = 0;
        for (uint64_t j = 0; j < tmp.size();j++)
        {
            double dist = sqrtf64(powf64(tmp[j].x - travellerX, 2.0) + powf64(tmp[j].z - travellerZ, 2.0));
            if (dist < winningDist)
            {
                winningDist = dist;
                winner = tmp[j];
                winningIndex = j;
            }
        }
        tmp.erase(tmp.begin() + winningIndex);
        travellerX = winner.x;
        travellerZ = winner.z;
        result.push_back(winner);
    }
    for (const auto& a : result)
    {
        std::cout << a.x << ", " << a.z << std::endl;
    }
    std::cout << result.size() << std::endl;

    return result;
}

int main() {
    // Set up window properties
    sf::RenderWindow window(sf::VideoMode(600, 600), "Elytra Sniper");
    window.setPosition({600, 20});
    window.setFramerateLimit(60);
    bool res = ImGui::SFML::Init(window); // function is marked [[NODISCARD]]
    if (!res){
        return 1;
    }

    // Default variables. Most of these can be changed in the program.
    int mapScale = 8;
    MCVersion mc = MC_1_20;
    uint64_t seed = 2438515238773172647;
    int64_t playerX = 2500, playerZ = 2500;
    int64_t px = playerX, pz = playerZ;

    // Find and load the structures around the player.
    findStructuresAround(seed, playerX, playerZ, mc);
    std::vector<CityLocation> cities { readCitiesAround(seed, playerX, playerZ) };

    // Calculate Path
    std::vector<CityLocation> path { GeneratePath(10, cities, playerX, playerZ) };

    // More map set up
    mapScale = optimalScale(path, playerX, playerZ);
    int64_t startX = playerX - (75 * mapScale);
    int64_t startZ = playerZ - (75 * mapScale);

    // Do expensive rendering once and save to a texture.
    es::colorMap_t colorMap { es::generate_ColorMap(mc, seed, startX, startZ, mapScale) }; 
    std::shared_ptr<sf::Texture> map { es::generate_map(window, colorMap) };
    sf::Sprite mapSprite(*map, sf::IntRect(0, 0, 300, 300));

    // Load icons
    sf::Texture city_icon;
    city_icon.loadFromFile(PROJECT_DIR("assets/city.png"));
    sf::Texture ship_icon;
    ship_icon.loadFromFile(PROJECT_DIR("assets/ship.png"));
    sf::Texture player_icon;
    player_icon.loadFromFile(PROJECT_DIR("assets/player.png"));

    
    // Start loop
    es::ImGuiTheme();
    sf::Clock deltaClock;
    while (window.isOpen()) 
    {   

        // Shutdown when the window is closed or escape is pressed.
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

        // Draw the screen
        window.clear();

        window.draw(mapSprite);     // Draw the saved map texture

        // Draw lines connecting the cities
        for (int i = 0; i < path.size() - 1; i++) {
            es::drawPath(
                window,
                { static_cast<float>(path.at(i  ).x - startX) / mapScale*4.f, static_cast<float>(path.at(i  ).z - startZ) / mapScale*4.f },
                { static_cast<float>(path.at(i+1).x - startX) / mapScale*4.f, static_cast<float>(path.at(i+1).z - startZ) / mapScale*4.f }
            );
        }
        // Draw one more line to the player
        es::drawPath(
            window,
            { static_cast<float>(playerX - startX) / mapScale*4.f, static_cast<float>(playerZ - startZ) / mapScale*4.f },
            { static_cast<float>(path.at(0).x - startX) / mapScale*4.f, static_cast<float>(path.at(0).z - startZ) / mapScale*4.f }
        );

        // Some variables control drawing
        int iconScale = 10;
        int mx = (playerX - startX) / mapScale*4 - iconScale/2;
        int mz = (playerZ - startZ) / mapScale*4 - iconScale/2;

        // Draw each city
        for (const CityLocation& city : cities) {
            int mx = (city.x - startX) / mapScale*4 - iconScale/2;
            int mz = (city.z - startZ) / mapScale*4 - iconScale/2;

            sf::Sprite sprite;
            sprite.setTexture((city.hasShip) ? ship_icon : city_icon);
            sprite.setScale(3, 3);
            sprite.setPosition(mx, mz);
            window.draw(sprite);
        }

        // Draw the player dot
        sf::Sprite sprite;
        sprite.setTexture(player_icon);
        sprite.setScale(3, 3);
        sprite.setPosition(mx, mz);
        window.draw(sprite);

        // Draw the GUI
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
        ImVec4 popedCol = ImGui::GetStyle().Colors[ImGuiCol_Text];
        ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.f);
        ImGui::Begin("User Input", nullptr, window_flags);
        ImGui::GetStyle().Colors[ImGuiCol_Text] = popedCol;
        ImGui::PushItemWidth(150);
        ImGui::InputScalar("Seed", ImGuiDataType_U64, &seed, nullptr, nullptr, "%lu");
        ImGui::InputScalar("X", ImGuiDataType_S64, &px, nullptr, nullptr, "%ld");
        ImGui::InputScalar("Z", ImGuiDataType_S64, &pz, nullptr, nullptr, "%ld");
        static bool buttonPressed = false;
        if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) buttonPressed = true;
        ImGui::PopItemWidth();
        // When the regenerate function is clicked, update the values.
        if (ImGui::Button("Regenerate") || buttonPressed)
        {
            playerX = px;
            playerZ = pz;
            // Find and load the structures around the player.
            findStructuresAround(seed, playerX, playerZ, mc);
            cities = readCitiesAround(seed, playerX, playerZ);
            cities = filterCities(cities, false, true);

            mapScale = optimalScale(path, playerX, playerZ);

            startX = playerX - (75 * mapScale);
            startZ = playerZ - (75 * mapScale);

            // Do the expensive calculations again
            colorMap = es::generate_ColorMap(mc, seed, startX, startZ, mapScale);
            map =  es::generate_map(window, colorMap);
            mapSprite.setTexture(*map);

            buttonPressed = false;
        }
        if (ImGui::Button("Move to Next"))
        {
            // Move to next point.
            if (path.size() > 1) {
              playerX = path.at(0).x;
              playerZ = path.at(0).z;
              path.at(0).looted = true;

              std::cout << "Marking looted\n";
              markCityLooted(seed, path.at(0).x, path.at(0).z);

              startX = playerX - (75 * mapScale);
              startZ = playerZ - (75 * mapScale);

              cities = filterCities(cities, false, true);

              std::cout << "size: " << path.size() << "\n";

              path.erase(path.begin());
            }
            mapSprite.setTexture(*map);
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
