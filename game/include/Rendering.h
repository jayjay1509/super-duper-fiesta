#ifndef MICROMACHINE_GAME_INCLUDE_RENDERING_H_
#define MICROMACHINE_GAME_INCLUDE_RENDERING_H_

#include <SFML/Graphics.hpp>
#include "const.h"

class Renderer {
 private:
  sf::RenderWindow window_;

 public:
  Renderer(std::string_view name) {
    auto window_size = sf::VideoMode({kWindowWidth, kWindowLength});
    window_.create(window_size, name.data());
  }

  sf::RenderWindow& Window(){return window_;}

  void Clear(const sf::Color color = sf::Color::Black){window_.clear(color);}

  void Display(){window_.display();}

  void Draw(const sf::Drawable& drawable){window_.draw(drawable);}

  void Render(const std::vector<sf::Sprite>& sprites)
  {
    for (auto& sprite:sprites) {
      Draw(sprite);
    }
    Display();
  }
};

#endif //MICROMACHINE_GAME_INCLUDE_RENDERING_H_
