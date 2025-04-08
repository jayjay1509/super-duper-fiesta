
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

enum class Status {
  NOT_CONNECTED,
  CONNECTED
};

int main() {
  sf::RenderWindow window(sf::VideoMode({kWindowWidth, kWindowLength}), "hockey");
  window.setFramerateLimit(60);
  crackitos_physics::physics::PhysicsWorld physics_world_;


  crackitos_core::math::AABB worldBounds(
      crackitos_core::math::Vec2f(0, 0), // Coin inférieur gauche
      crackitos_core::math::Vec2f(kWindowWidthF, kWindowLengthF) // Coin supérieur droit (taille de la fenêtre)
  );

// Met à jour les limites du monde physique
  physics_world_.Initialize(worldBounds, true, crackitos_core::math::Vec2f(0, 0));

// Mur supérieur
  crackitos_physics::physics::Body topWallBody;
  topWallBody.set_position(crackitos_core::math::Vec2f(kWindowWidthF / 2.0f, 0.f));  // Position du mur supérieur
  topWallBody.set_mass(0.0f);  // Pas de masse pour un mur statique
  auto topWallHandle = physics_world_.CreateBody(topWallBody);

// Collider pour le mur supérieur
  crackitos_physics::physics::Collider topWallCollider(
      crackitos_core::math::AABB(crackitos_core::math::Vec2f(0.f, 0.f), crackitos_core::math::Vec2f(kWindowWidthF, 10.f)),
      0.0f,  // Pas de rebond
      1.0f,  // Friction élevée
      false, // Pas un trigger
      topWallHandle // Handle du corps du mur
  );
  physics_world_.CreateCollider(topWallHandle, topWallCollider);

// Mur inférieur
  crackitos_physics::physics::Body bottomWallBody;
  bottomWallBody.set_position(crackitos_core::math::Vec2f(kWindowWidthF / 2.0f, kWindowLengthF));  // Position du mur inférieur
  bottomWallBody.set_mass(0.0f);  // Pas de masse pour un mur statique
  auto bottomWallHandle = physics_world_.CreateBody(bottomWallBody);

// Collider pour le mur inférieur
  crackitos_physics::physics::Collider bottomWallCollider(
      crackitos_core::math::AABB(crackitos_core::math::Vec2f(0.f, 0.f), crackitos_core::math::Vec2f(kWindowWidthF, 10.f)),
      0.0f,  // Pas de rebond
      1.0f,  // Friction élevée
      false, // Pas un trigger
      bottomWallHandle // Handle du corps du mur
  );
  physics_world_.CreateCollider(bottomWallHandle, bottomWallCollider);



// Mur gauche
  crackitos_physics::physics::Body leftWallBody;
  leftWallBody.set_position(crackitos_core::math::Vec2f(0.f, kWindowLengthF / 2.0f));  // Position du mur gauche
  leftWallBody.set_mass(0.0f);  // Pas de masse pour un mur statique
  auto leftWallHandle = physics_world_.CreateBody(leftWallBody);

// Collider pour le mur gauche
  crackitos_physics::physics::Collider leftWallCollider(
      crackitos_core::math::AABB(crackitos_core::math::Vec2f(0.f, 0.f), crackitos_core::math::Vec2f(10.f, kWindowLengthF)),
      0.0f,  // Pas de rebond
      1.0f,  // Friction élevée
      false, // Pas un trigger
      leftWallHandle // Handle du corps du mur
  );
  physics_world_.CreateCollider(leftWallHandle, leftWallCollider);

// Mur droit
  crackitos_physics::physics::Body rightWallBody;
  rightWallBody.set_position(crackitos_core::math::Vec2f(kWindowWidthF, kWindowLengthF / 2.0f));  // Position du mur droit
  rightWallBody.set_mass(0.0f);  // Pas de masse pour un mur statique
  auto rightWallHandle = physics_world_.CreateBody(rightWallBody);

// Collider pour le mur droit
  crackitos_physics::physics::Collider rightWallCollider(
      crackitos_core::math::AABB(crackitos_core::math::Vec2f(0.f, 0.f), crackitos_core::math::Vec2f(10.f, kWindowLengthF)),
      0.0f,  // Pas de rebond
      1.0f,  // Friction élevée
      false, // Pas un trigger
      rightWallHandle // Handle du corps du mur
  );
  physics_world_.CreateCollider(rightWallHandle, rightWallCollider);

  sf::Texture texture;
  if (!texture.loadFromFile("data/sprites/test_map.png")) {
    return -1; // Vérifie si le fichier est bien chargé
  }
  sf::Sprite sprite(texture);
  sprite.setPosition(sf::Vector2f (0,0));

  PlayerController player(100,physics_world_);


  if (!ImGui::SFML::Init(window)) {
    std::cerr<<"window creation error";
  }

  bool isOpen = true;
  sf::Clock deltaClock;
  Status status = Status::NOT_CONNECTED;

  std::vector<std::string> receivedMessages;

  ImGui::SetNextWindowSize({300, 100}, ImGuiCond_Always);
  ImGui::SetNextWindowPos({20.0f, 20.0f}, ImGuiCond_Always);




  while (isOpen) {
    physics_world_.Update(deltaClock.getElapsedTime().asSeconds());

    while (const std::optional event = window.pollEvent()) {
      ImGui::SFML::ProcessEvent(window, *event);
      if (event->is<sf::Event::Closed>()) {
        isOpen = false;
      }
    }

    sf::Vector2f direction(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
      direction.y = -1.f;  // Déplacement vers le haut
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
      direction.y = 1.f;  // Déplacement vers le bas
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
      direction.x = -1.f;  // Déplacement vers la gauche
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
      direction.x = 1.f;  // Déplacement vers la droite
    }

// Appliquer le mouvement avec une vitesse
    player.Move(direction);


    player.Update(deltaClock.getElapsedTime().asSeconds());


    sf::RectangleShape playerShape(sf::Vector2f(50.f, 50.f)); // Taille du carré
    playerShape.setFillColor(sf::Color::Green);
    sf::Vector2f playerPos(static_cast<float>(player.GetPosition().x),
                           static_cast<float>(player.GetPosition().y));
    playerShape.setPosition(playerPos);


    ImGui::SFML::Update(window, deltaClock.restart());
    auto [x, y] = window.getSize();

    ImGui::Begin("Simple Chat", nullptr, ImGuiWindowFlags_NoTitleBar);

    ImGui::End();
    window.clear();

    ImGui::SFML::Render(window);
    window.draw(sprite);


    sf::RectangleShape topWallShape(sf::Vector2f(kWindowWidthF, 10.f));  // Largeur de la fenêtre et hauteur du mur
    topWallShape.setFillColor(sf::Color::Red);  // Choisissez la couleur que vous préférez
    sf::Vector2f topWallPosition(topWallBody.position().x, topWallBody.position().y);
    topWallShape.setPosition(topWallPosition);  // Positionner le mur au sommet de l'écran

// Mur inférieur
    sf::RectangleShape bottomWallShape(sf::Vector2f(kWindowWidthF, 10.f));  // Largeur de la fenêtre et hauteur du mur
    bottomWallShape.setFillColor(sf::Color::Red);  // Couleur du mur
    sf::Vector2f bottomWallPosition(bottomWallBody.position().x, bottomWallBody.position().y);
    bottomWallShape.setPosition(bottomWallPosition);  // Positionner le mur en bas

// Mur gauche
    sf::RectangleShape leftWallShape(sf::Vector2f(10.f, kWindowLengthF));  // Hauteur de la fenêtre et largeur du mur
    leftWallShape.setFillColor(sf::Color::Red);  // Couleur du mur
    sf::Vector2f leftWallPosition(leftWallBody.position().x, rightWallBody.position().y);
    leftWallShape.setPosition(leftWallPosition);  // Positionner le mur sur le côté gauche

// Mur droit
    sf::RectangleShape rightWallShape(sf::Vector2f(10.f, kWindowLengthF));  // Hauteur de la fenêtre et largeur du mur
    rightWallShape.setFillColor(sf::Color::Red);  // Couleur du mur
    sf::Vector2f rightWallPosition(rightWallBody.position().x, rightWallBody.position().y);
    rightWallShape.setPosition(rightWallPosition);  // Positionner le mur sur le côté droit

// Ensuite, vous pouvez dessiner les murs dans la fenêtre, comme suit :
    window.draw(topWallShape);
    window.draw(bottomWallShape);
    window.draw(leftWallShape);
    window.draw(rightWallShape);


    window.draw(playerShape);


    window.display();
  }

  ImGui::SFML::Shutdown();
}