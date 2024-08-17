#include "es_frontend.hpp"
#include "generator.h"

namespace es{
colorMap_t generate_ColorMap (const MCVersion& mc, uint64_t& seed, const int64_t& sx, const int64_t& sz)
{
    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, DIM_END, seed);
    
    colorMap_t CM { std::make_shared<std::array<std::array<sf::Color, 150>, 150>>() };
    for (uint64_t x = 0; x < CM->size(); x++)
    {
        for (uint64_t z = 0; z < CM->at(x).size(); z++) 
        {
            int biomeID = getBiomeAt(&g, 1, sx+(x*8), 63, sz+(z*8)); // scale, x, y, z 
            if (biomeID == small_end_islands)    
                CM->at(x).at(z) = color_void;
            else if (biomeID == end_barrens)
                CM->at(x).at(z) = color_barr;
            else if (biomeID == end_midlands)
                CM->at(x).at(z) = color_midl;
            else if (biomeID == end_highlands)
                CM->at(x).at(z) = color_high;
        }
    }
    return CM;
}

std::shared_ptr<sf::RenderTexture> generate_map (const sf::Window& win, const colorMap_t& CM)
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
