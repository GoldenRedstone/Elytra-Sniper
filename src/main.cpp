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

namespace ES{
    const sf::Color color_void { 0, 0, 42 };
    const sf::Color color_barr { 90, 90, 72 };
    const sf::Color color_midl { 235, 248, 182 };
    const sf::Color color_high { 195, 189, 89 };
    using colorMap_t = std::shared_ptr<std::array<std::array<sf::Color, 150>, 150>>;

    ES::colorMap_t generate_ColorMap ()
    {
        Generator g;
        uint64_t seed = 2438515238773172647;
        setupGenerator(&g, MC_1_20, 0);
        applySeed(&g, DIM_END, seed);
        
        ES::colorMap_t CM { std::make_shared<std::array<std::array<sf::Color, 150>, 150>>() };
        for (uint64_t x = 0; x < CM->size(); x++)
        {
            for (uint64_t z = 0; z < CM->at(x).size(); z++) 
            {
                int biomeID = getBiomeAt(&g, 1, -2500+(x*16), 63, 4000+(z*16)); // scale, x, y, z 
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

class CityLocation {
public:
    int x;
    int y;
    bool hasShip;
    bool looted;
};

std::vector<CityLocation> parseCSVFile(std::string filename) {
    std::ifstream input{filename};

    if (!input.is_open()) {
    std::cerr << "Couldn't read file: " << filename << "\n";
        return {};
    }

    std::vector<CityLocation> cities;

    std::vector<std::string> headRow;

    CityLocation newCity;
    newCity.x = -1200;
    newCity.y = 4600;
    newCity.hasShip = 0;
    newCity.looted = 0;

    cities.push_back(newCity);

    for (std::string line; std::getline(input, line);) {
        std::istringstream ss(std::move(line));
        std::vector<std::string> row;

        for (std::string value; std::getline(ss, value, ',');) {
            row.push_back(std::move(value));
        }

        if (headRow.empty()) {
            // Skip this row
            headRow = row;
            continue;
        }

        CityLocation newCity;
        newCity.x = std::stoi(row.at(0));
        newCity.y = std::stoi(row.at(1));
        newCity.hasShip = row.at(2) == "1";
        newCity.looted = row.at(3) == "1";

        cities.push_back(newCity);
    }
    return cities;
}

std::vector<CityLocation> readCitiesAround(uint64_t seed, int x, int z) {
    std::vector<CityLocation> mommy;
    std::vector<CityLocation> child;
    std::ostringstream filename;

    int r = 5000;
    int rx = x/r;
    int rz = z/r;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            std::cout << "Loading chunk " << i << " " << j << "\n";
            filename << "searched/" << seed << "." << r*(rx+i) << "." << r*(rz+j) << ".csv";
            std::cout << filename.str() << "\n";
            child = parseCSVFile(filename.str());
            mommy.insert(mommy.end(), child.begin(), child.end());
            filename.str(std::string());
        }
    }
    

    return mommy;
}

int main() {
    // std::vector<CityLocation> cities = parseCSVFile("searched/2438515238773172647.-5000.0.csv");
    std::vector<CityLocation> cities = readCitiesAround(2438515238773172647, -1200, 4600);

    // Print out our table
    for (const CityLocation& city : cities) {
        std::cout << city.x << ", " << city.y << ", " << city.hasShip << ", " << city.looted;
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

        for (const CityLocation& city : cities) {
            int mx = (city.x + 2500) / 16*4;
            int my = (city.y - 4000) / 16*4;
            sf::CircleShape circle;
            circle.setRadius(10);
            if (city.hasShip) {
                circle.setFillColor(sf::Color::Green);
            } else {
                circle.setFillColor(sf::Color::Red);
            }
            circle.setPosition(mx, my);
            window.draw(circle);
        }

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
