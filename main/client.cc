#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <crackitos_physics/physics/include/body.h>

#include <iostream>

#include "const.h"
#include "physics_world.h"
#include "PlayerController.h"

enum class Status {
  NOT_CONNECTED,
  CONNECTED
};

sf::RenderWindow window;
crackitos_physics::physics::PhysicsWorld physics_world_;
PlayerController player_controller_;
sf::Texture texture;
sf::Sprite sprite(texture);
sf::Clock deltaClock;

bool init() {

  window.create(sf::VideoMode({kWindowWidth, kWindowLength}), "super-duper-fiesta");
  window.setFramerateLimit(60);


  // Chargement de la texture
  if (!texture.loadFromFile("data/map/test_map.png")) {
    std::cerr << "Erreur de chargement de la texture." << std::endl;
    return false;
  }
  sprite.setTexture(texture);
  sprite.setPosition(sf::Vector2f(0, 0));


  if (!ImGui::SFML::Init(window)) {
    std::cerr << "Erreur de création de la fenêtre ImGui." << std::endl;
    return false;
  }

  return true;
}

void update() {

  while (const std::optional event = window.pollEvent()) {
    ImGui::SFML::ProcessEvent(window, *event);
    if (event->is<sf::Event::Closed>()) {
      window.close();
    }
  }

  ImGui::SFML::Update(window, deltaClock.restart());

  ImGui::Begin("Simple Chat", nullptr, ImGuiWindowFlags_NoTitleBar);
  ImGui::End();
}

void last_update() {

  window.clear();
  ImGui::SFML::Render(window);


  window.draw(sprite);


  window.display();
}

int main() {
  if (!init()) {
    return -1;
  }

  bool isOpen = true;
  Status status = Status::NOT_CONNECTED;
  std::vector<std::string> receivedMessages;

  while (isOpen) {
    update();
    last_update();
  }


  ImGui::SFML::Shutdown();

  return 0;
}
