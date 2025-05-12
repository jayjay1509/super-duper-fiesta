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
#include "my_client.h"

enum class Status {
  NOT_CONNECTED,
  CONNECTED
};

int main() {
  sf::RenderWindow window(sf::VideoMode({kWindowWidth, kWindowLength}), "super-duper-fiesta");
  window.setFramerateLimit(60);
  crackitos_physics::physics::PhysicsWorld physics_world_;
  std::vector<Bullet> bullets;

  MyClient client;
  ExitGames::LoadBalancing::ClientConstructOptions options;
  SDF::NetworkManager::Begin(&client, options);

  sf::Clock shootClock;
  const float shootCooldown = 1.f;


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
      crackitos_core::math::Vec2f(50.f, 400.f)
  );

  // Chargement de la texture
  sf::Texture texture;
  if (!texture.loadFromFile("data/sprites/test_map.png")) {
    return -1;
  }
  sf::Sprite sprite(texture);
  sprite.setPosition(sf::Vector2f(0, 0));

  PlayerController player(100,sf::Vector2f(100, 100), physics_world_);
  PlayerController player2(100,sf::Vector2f(300, 300), physics_world_);

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

    int remoteNr = client.getRemotePlayerNr();


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
    float directionX = 0;
    float directionY = 0;

    sf::Vector2f direction2(0, 0);



    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) directionY = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) directionY = 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) directionX = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) directionX = 1.f;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)
        && shootClock.getElapsedTime().asSeconds() >= shootCooldown)
    {

      shootClock.restart();


      sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
      sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

      sf::Vector2f playerPos(player.GetPosition().x, player.GetPosition().y);
      sf::Vector2f dir = mouseWorldPos - playerPos;
      float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
      if (len != 0)
        dir /= len;

      crackitos_core::math::Vec2f vecDir{ dir.x, dir.y };
      player.Shoot(vecDir);
    }


    sf::Vector2f otherDir = client.getDirection(remoteNr);


    player.Move({directionX,directionY});
    player2.Move(otherDir);

    player.Update(deltaClock.getElapsedTime().asSeconds());

    player2.Update(deltaClock.getElapsedTime().asSeconds());

    // Rendu du joueur
    sf::RectangleShape playerShape(sf::Vector2f(50.f, 50.f));
    playerShape.setFillColor(sf::Color::Green);
    auto playerPos = player.GetPosition();
    // Conversion de Vec2f en Vector2f
    sf::Vector2f visualPos(playerPos.x - 25.f, playerPos.y - 25.f);
    playerShape.setPosition(visualPos);

    sf::RectangleShape playerShape2(sf::Vector2f(50.f, 50.f));
    playerShape2.setFillColor(sf::Color::Blue);
    auto playerPos2 = player2.GetPosition();
    sf::Vector2f visualPos2(playerPos2.x - 25.f, playerPos2.y - 25.f);
    playerShape2.setPosition(visualPos2);



    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
    sf::Vector2f tankCenter(playerPos.x, playerPos.y);


    sf::Vertex line[] = {
        sf::Vertex(tankCenter, sf::Color::Red),
        sf::Vertex(mousePos, sf::Color::Red)
    };


    sf::CircleShape aimMarker(3.f);
    aimMarker.setFillColor(sf::Color::Red);
    aimMarker.setOrigin(sf::Vector2f (3.f, 3.f));
    aimMarker.setPosition(mousePos);


    for (auto& bullet : bullets) {
      bullet.Update(deltaClock.getElapsedTime().asSeconds());
      std::cout << "Updated Bullet Position: " << bullet.GetPosition().x << ", " << bullet.GetPosition().y << std::endl;
    }


    char buf[64];
    std::snprintf(buf, sizeof(buf), "%f,%f", directionX, directionY);
    ExitGames::Common::JString jsDir(buf);
    SDF::NetworkManager::GetLoadBalancingClient()
        .opRaiseEvent(false, jsDir, 2);



    ImGui::SFML::Update(window, deltaClock.restart());
    ImGui::Begin("Simple Chat", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::End();


    window.clear();


    for (auto& wallShape : wallShapes) {
      window.draw(wallShape);
    }

    for (auto& bullet : bullets) {
      if (bullet.IsActive()) {
        std::cout << "Bullet Position: " << bullet.GetPosition().x << ", " << bullet.GetPosition().y << std::endl;


        sf::CircleShape shape(10.f);  // Rayon de la balle
        shape.setPosition(sf::Vector2f( bullet.GetPosition().x, bullet.GetPosition().y));  // Position de la balle
        shape.setFillColor(sf::Color::Red);
        window.draw(shape);
      }
    }

    window.draw(playerShape);
    window.draw(playerShape2);
    player.Draw(window);
    player2.Draw(window);


    window.draw(line,2, sf::PrimitiveType::Lines);
    window.draw(aimMarker);
    ImGui::SFML::Render(window);
    window.display();
  }
  ImGui::SFML::Shutdown();
  return 0;
}