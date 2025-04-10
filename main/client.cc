#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <iostream>
#include "const.h"
#include "player_controler.h"
#include "physics_world.h"
#include "WallFactory.h"

enum class Status {
  NOT_CONNECTED,
  CONNECTED
};

int main() {
  sf::RenderWindow window(sf::VideoMode({kWindowWidth, kWindowLength}), "super-duper-fiesta");
  window.setFramerateLimit(60);
  crackitos_physics::physics::PhysicsWorld physics_world_;
  std::vector<Bullet> bullets;

  // Initialisation du monde physique
  crackitos_core::math::AABB worldBounds(
      crackitos_core::math::Vec2f(0, 0),
      crackitos_core::math::Vec2f(kWindowWidthF, kWindowLengthF)
  );
  physics_world_.Initialize(worldBounds, false, crackitos_core::math::Vec2f(0, 0));

  WallFactory wallFactory(physics_world_);

// Mur supérieur
  auto topWallHandle = wallFactory.CreateWall(
      crackitos_core::math::Vec2f(kWindowWidthF / 2.0f, 0.f),
      crackitos_core::math::Vec2f(kWindowWidthF, 50.f)
  );

// Mur inférieur
  auto bottomWallHandle = wallFactory.CreateWall(
      crackitos_core::math::Vec2f(kWindowWidthF / 2.0f, kWindowLengthF),
      crackitos_core::math::Vec2f(kWindowWidthF, 50.f)
  );


  auto leftWallHandle = wallFactory.CreateWall(
      crackitos_core::math::Vec2f(0.f, kWindowLengthF / 2.0f),
      crackitos_core::math::Vec2f(50.f, kWindowLengthF)
  );


  auto rightWallHandle = wallFactory.CreateWall(
      crackitos_core::math::Vec2f(kWindowWidthF, kWindowLengthF / 2.0f),
      crackitos_core::math::Vec2f(50.f, kWindowLengthF)
  );

  auto centerHorizontalWallHandle = wallFactory.CreateWall(
      crackitos_core::math::Vec2f(kWindowWidthF / 2.0f, kWindowLengthF / 2.0f),
      crackitos_core::math::Vec2f(400.f, 50.f)  // Longueur du mur horizontal
  );

// Mur central vertical
  auto centerVerticalWallHandle = wallFactory.CreateWall(
      crackitos_core::math::Vec2f(kWindowWidthF / 2.0f, kWindowLengthF / 2.0f),
      crackitos_core::math::Vec2f(50.f, 400.f)  // Longueur du mur vertical
  );

  // Chargement de la texture
  sf::Texture texture;
  if (!texture.loadFromFile("data/sprites/test_map.png")) {
    return -1;
  }
  sf::Sprite sprite(texture);
  sprite.setPosition(sf::Vector2f(0, 0));

  PlayerController player(100, physics_world_);

  if (!ImGui::SFML::Init(window)) {
    std::cerr << "window creation error";
  }

  bool isOpen = true;
  sf::Clock deltaClock;
  Status status = Status::NOT_CONNECTED;

  ImGui::SetNextWindowSize({300, 100}, ImGuiCond_Always);
  ImGui::SetNextWindowPos({20.0f, 20.0f}, ImGuiCond_Always);

  std::vector<sf::RectangleShape> wallShapes;

  // Mur supérieur
  sf::RectangleShape topWall(sf::Vector2f(kWindowWidthF, 50.f));
  topWall.setFillColor(sf::Color::Red);
  topWall.setOrigin(sf::Vector2f(kWindowWidthF / 2.0f, 25.f)); // Centre de la forme
  topWall.setPosition(sf::Vector2f(kWindowWidthF / 2.0f, 0.f)); // Position physique (centre du mur)
  wallShapes.push_back(topWall);

// Mur inférieur
  sf::RectangleShape bottomWall(sf::Vector2f(kWindowWidthF, 50.f));
  bottomWall.setFillColor(sf::Color::Red);
  bottomWall.setOrigin(sf::Vector2f(kWindowWidthF / 2.0f, 25.f)); // Centre de la forme
  bottomWall.setPosition(sf::Vector2f(kWindowWidthF / 2.0f, kWindowLengthF)); // Position physique (centre du mur)
  wallShapes.push_back(bottomWall);

// Mur gauche
  sf::RectangleShape leftWall(sf::Vector2f(50.f, kWindowLengthF));
  leftWall.setFillColor(sf::Color::Red);
  leftWall.setOrigin(sf::Vector2f(25.f, kWindowLengthF / 2.0f)); // Centre de la forme
  leftWall.setPosition(sf::Vector2f(0.f, kWindowLengthF / 2.0f)); // Position physique (centre du mur)
  wallShapes.push_back(leftWall);

// Mur droit
  sf::RectangleShape rightWall(sf::Vector2f(50.f, kWindowLengthF));
  rightWall.setFillColor(sf::Color::Red);
  rightWall.setOrigin(sf::Vector2f(25.f, kWindowLengthF / 2.0f)); // Centre de la forme
  rightWall.setPosition(sf::Vector2f(kWindowWidthF, kWindowLengthF / 2.0f)); // Position physique (centre du mur)
  wallShapes.push_back(rightWall);

// Mur central horizontal
  sf::RectangleShape horizontalWall(sf::Vector2f(400.f, 50.f));  // Taille : 400 x 50
  horizontalWall.setFillColor(sf::Color::Red);
  horizontalWall.setOrigin(sf::Vector2f(400.f / 2.0f, 50.f / 2.0f)); // Centre de la forme
  horizontalWall.setPosition(sf::Vector2f(kWindowWidthF / 2.0f, kWindowLengthF / 2.0f)); // Position physique (centre du mur)
  wallShapes.push_back(horizontalWall);

// Mur central vertical
  sf::RectangleShape verticalWall(sf::Vector2f(50.f, 400.f));  // Taille : 50 x 400
  verticalWall.setFillColor(sf::Color::Red);
  verticalWall.setOrigin(sf::Vector2f(50.f / 2.0f, 400.f / 2.0f)); // Centre de la forme
  verticalWall.setPosition(sf::Vector2f(kWindowWidthF / 2.0f, kWindowLengthF / 2.0f)); // Position physique (centre du mur)
  wallShapes.push_back(verticalWall);



  while (isOpen) {
    physics_world_.Update(deltaClock.getElapsedTime().asSeconds());

    // Gestion des événements
    while (const std::optional event = window.pollEvent()) {
      ImGui::SFML::ProcessEvent(window, *event);
      if (event->is<sf::Event::Closed>()) {
        isOpen = false;
      }
    }

    // Gestion des inputs
    sf::Vector2f direction(0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) direction.y = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) direction.y = 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) direction.x = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) direction.x = 1.f;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
      sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
      sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos); // prend en compte la caméra

      // Position du joueur dans SFML
      sf::Vector2f playerPos(player.GetPosition().x, player.GetPosition().y);

      // Vecteur directionnel

      sf::Vector2f dir = mouseWorldPos - playerPos;
      crackitos_core::math::Vec2f vecDir(dir.x, dir.y);

      // Normaliser
      float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
      if (len != 0)
        dir /= len;

      player.Shoot(vecDir);
      //std::cout << "pan\n" << vecDir.x << ":" << vecDir.y << "pan\n";
    }
    player.Move(direction);
    player.Update(deltaClock.getElapsedTime().asSeconds());

    // Rendu du joueur
    sf::RectangleShape playerShape(sf::Vector2f(50.f, 50.f));
    playerShape.setFillColor(sf::Color::Green);
    auto playerPos = player.GetPosition();
    // Conversion de Vec2f en Vector2f
    sf::Vector2f visualPos(playerPos.x - 25.f, playerPos.y - 25.f);
    playerShape.setPosition(visualPos);

    // Indicateur de visée
    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
    sf::Vector2f tankCenter(playerPos.x, playerPos.y);

    // Ligne de visée
    sf::Vertex line[] = {
        sf::Vertex(tankCenter, sf::Color::Red),
        sf::Vertex(mousePos, sf::Color::Red)
    };

    // Marqueur de visée
    sf::CircleShape aimMarker(3.f);
    aimMarker.setFillColor(sf::Color::Red);
    aimMarker.setOrigin(sf::Vector2f (3.f, 3.f)); // Centrer le marqueur
    aimMarker.setPosition(mousePos);


    for (auto& bullet : bullets) {
      bullet.Update(deltaClock.getElapsedTime().asSeconds());
      std::cout << "Updated Bullet Position: " << bullet.GetPosition().x << ", " << bullet.GetPosition().y << std::endl;
    }


    // Dessiner les balles actives



    // Mise à jour de l'UI
    ImGui::SFML::Update(window, deltaClock.restart());
    ImGui::Begin("Simple Chat", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::End();

    // Rendu
    window.clear();



    //window.draw(sprite);
    for (auto& wallShape : wallShapes) {
      window.draw(wallShape);
    }

    window.draw(playerShape);

    for (auto& bullet : bullets) {
      if (bullet.IsActive()) {
        std::cout << "Bullet Position: " << bullet.GetPosition().x << ", " << bullet.GetPosition().y << std::endl;

        // Dessinez la balle avec SFML
        sf::CircleShape shape(10.f);  // Rayon de la balle
        shape.setPosition(sf::Vector2f( bullet.GetPosition().x, bullet.GetPosition().y));  // Position de la balle
        shape.setFillColor(sf::Color::Red);
        window.draw(shape);
      }
    }
    player.Draw(window);


    window.draw(line,2, sf::PrimitiveType::Lines);
    window.draw(aimMarker);
    ImGui::SFML::Render(window);
    window.display();
  }
  ImGui::SFML::Shutdown();
  return 0;
}